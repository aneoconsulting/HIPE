#pragma once


#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>
#include <filter/data/PatternData.h>

namespace filter
{
	namespace algos
	{
		void cropper_mouse_call(int event, int x, int y, int f, void* cvCropperData);

		class FILTER_EXPORT Cropper : public IFilter
		{
			std::shared_ptr<data::PatternData> _pattern;

		public:
			class CVCropperData {
			public:
				bool clicked;
				bool drawing;
				int index;
				cv::String windowName;
				std::vector<cv::Point> rectangle;
				cv::Mat sandBoxImage;
				data::PatternData pattern;


			public:
				CVCropperData(const data::PatternData & pat) : pattern(pat), clicked(false), drawing(false), index(0)
				{
					
				}
			};

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageData, data::PatternData);

			

			REGISTER(Cropper, ()), _connexData(data::INDATA)
			{

				waitkey = 10;
			}

			REGISTER_P(int, waitkey);
			~Cropper()
			{

			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		private:
			void CropperAreaDrawer(filter::algos::Cropper::CVCropperData & cvCropperData)
			{
				cv::namedWindow(_name, cv::WINDOW_NORMAL);

				cvCropperData.windowName = _name;
				
				cvCropperData.clicked = false;
				cv::setMouseCallback(_name, cropper_mouse_call, &cvCropperData);

				cvCropperData.pattern.imageRequest().getMat().copyTo(cvCropperData.sandBoxImage);

				while (1) {
					cv::imshow(_name, cvCropperData.sandBoxImage);
					auto ret = cv::waitKey(30);

					if (ret == 27) {
						break;
					}
					if (ret == 13) // Return pressed
					{
						std::cout << "Keyboard has been pressed : " << ret << std::endl;
						break;
					}
					if (ret > 0)
					{
						std::cout << "unkonwn Keyboard has been pressed : " << ret << std::endl;
					}
				}
				cv::destroyWindow(_name);
			
			}

		public:


			HipeStatus process()
			{
				if (_pattern)
				{
					data::ImageData image = _connexData.pop();
					(*_pattern) << image;
					data::PatternData pattern(image);
					(*_pattern).copyTo(pattern);
					_connexData.push(pattern);

					return OK;
				}
				cv::namedWindow(_name);

				if (_connexData.size() == 0)
					throw HipeException("There is no data to display coming from the parent node [NAME]");

				data::ImageData image = _connexData.pop();
				auto myImage = image.getMat();
				if(myImage.rows <= 0 || myImage.cols <= 0)
					throw HipeException("Image to show doesn't data");
				data::PatternData pattern = data::PatternData(image);
				CVCropperData cvCropperData(pattern);
				
				CropperAreaDrawer(cvCropperData);

				pattern.crops() << cvCropperData.rectangle;
				cv::Mat patImage;
				cvCropperData.pattern.imageRequest().getMat().copyTo(patImage);

				pattern.crops() << patImage;

				
				_connexData.push(pattern);
				

				return OK;
			}

			void dispose()
			{
				Model::dispose();
				cv::destroyWindow(_name);
			}
		};

		ADD_CLASS(Cropper, waitkey);
	}
}
