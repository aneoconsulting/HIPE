#pragma once


#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>
#include <filter/data/PatternData.h>
#include "core/queue/ConcurrentQueue.h"

namespace filter
{
	namespace algos
	{
		void cropper_mouse_call(int event, int x, int y, int f, void* cvCropperData);

		class FILTER_EXPORT Cropper : public IFilter
		{
		public:
			class CVCropperData 
			{
			public:
				std::atomic<bool> clicked;
				std::atomic<bool> drawing;
				int index;
				cv::String windowName;
				std::vector<cv::Point> rectangle;
				cv::Mat sandBoxImage;
				cv::Mat sandBoxImage_backup;
				data::PatternData pattern;
				std::atomic<bool> started;

				core::queue::ConcurrentQueue<std::vector<cv::Point>> arrayCrop;

			public:
				CVCropperData(const data::PatternData & pat) : clicked(false), drawing(false), index(0), pattern(pat)
				{
					started = false;
				}

				CVCropperData(data::ImageData &image) : clicked(false), drawing(false), index(0), pattern(image)
				{
					started = false;
				}

				CVCropperData() : clicked(false), drawing(false), index(0)
				{
					started = false;
				}

				CVCropperData &operator<<(data::ImageData &inputImage)
				{
					pattern << inputImage;

					return *this;
				}

				CVCropperData &operator<<(cv::Mat inputImage)
				{
					
					pattern << data::ImageData(inputImage);

					return *this;
				}

			};

			std::shared_ptr<data::PatternData> _pattern;
			std::shared_ptr<boost::thread> _task;
			std::shared_ptr<CVCropperData> _cvCropperData;

		
			CONNECTOR(data::ImageData, data::PatternData);

			

			REGISTER(Cropper, ()), _connexData(data::INDATA)
			{
				_cvCropperData = std::make_shared<CVCropperData>();
				wait = false;
			}

			REGISTER_P(bool, wait);
			~Cropper()
			{

			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		private:
			void windowTask(std::shared_ptr<filter::algos::Cropper::CVCropperData> cvCropperData)
			{
				cv::namedWindow(_name, cv::WINDOW_NORMAL);
				cv::setMouseCallback(_name, cropper_mouse_call, &_cvCropperData);

				while (_cvCropperData->started) {
					cv::imshow(_name, cvCropperData->sandBoxImage);
					auto ret = cv::waitKey(30);

					if (ret == 27) {
						cv::destroyWindow(_name);
						break;
					}
					if (ret == 13 || ret == ' ') // Return or space pressed
					{
						std::cout << "Keyboard has been pressed : " << ret << std::endl;

						

						cv::Mat patImage;
						cvCropperData->sandBoxImage_backup.copyTo(patImage);

						//Populate SquareCrops
						cvCropperData->pattern.crops().addPair(cvCropperData->rectangle, patImage);

						cvCropperData->arrayCrop.push(cvCropperData->rectangle);

						cv::destroyWindow(_name);
						break;
					}
					if (ret > 0)
					{
						std::cout << "unkonwn Keyboard has been pressed : " << ret << std::endl;
					}
				}
			}

			void CropperAreaDrawer()
			{
				

				
				//If the user doesn't yet click then refresh the sandbox
				if (!_cvCropperData->clicked)
				{
					_cvCropperData->pattern.imageRequest().getMat().copyTo(_cvCropperData->sandBoxImage);
				}
				if (!_task)
				{
					_cvCropperData->started = true;
					_cvCropperData->windowName = _name;
					
					_cvCropperData->clicked = false;
					
					_task = std::make_shared<boost::thread>(boost::bind(&Cropper::windowTask, this, _cvCropperData));
				}

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
				

				if (_connexData.size() == 0)
					throw HipeException("There is no data to display coming from the parent node [NAME]");

				data::ImageData image = _connexData.pop();
				auto myImage = image.getMat();
				if(myImage.rows <= 0 || myImage.cols <= 0)
					throw HipeException("Image to show doesn't data");
				
				
				
				*(_cvCropperData) << myImage;

				CropperAreaDrawer();
				std::vector<cv::Point> rect;
				if (wait)
				{
					_cvCropperData->arrayCrop.wait_and_pop(rect);
					_connexData.push(_cvCropperData->pattern);
					_pattern = std::make_shared<data::PatternData>(_cvCropperData->pattern);
				}
				else if (_cvCropperData->arrayCrop.trypop_until(rect, 10) == false)
				{
					data::PatternData pattern(image);

					_connexData.push(pattern);

				}
				else
				{
					_connexData.push(_cvCropperData->pattern);
					_pattern = std::make_shared<data::PatternData>(_cvCropperData->pattern);
				}

				return OK;
			}

			void dispose()
			{
				Model::dispose();
				if (_task) 
					_cvCropperData->started = false;
				cv::destroyWindow(_name);
			}
		};

		ADD_CLASS(Cropper, wait);
	}
}
