#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class ShowVideo : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(ShowVideo, ()), _connexData(data::INOUT)
			{
				waitkey = 10;
				fullscreen = false;
				hasWindow = false;
			}

			REGISTER_P(int, waitkey);
			REGISTER_P(bool, fullscreen);

			std::atomic<bool> hasWindow;


			~ShowVideo()
			{
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			HipeStatus process()
			{
				//cv::namedWindow(_name, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO);
				if (!hasWindow.exchange(true))
				{
					if (fullscreen)
					{
						cv::namedWindow(_name, CV_WINDOW_FULLSCREEN);
					}
					else
					{
						cv::namedWindow(_name, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO);
					}
				}

				if (_connexData.size() == 0)
					throw HipeException("There is no data to display coming from the parent node [NAME]");

				data::ImageArrayData images = _connexData.pop();


				//Resize all images coming from the same parent
				for (auto& myImage : images.Array())
				{
					if (myImage.rows <= 0 || myImage.cols <= 0)
						throw HipeException("Image to show doesn't data");
					::cv::imshow(_name, myImage);

					if (waitkey >= 0)
						cvWaitKey(waitkey);
				}

				return OK;
			}

			void dispose()
			{
				Model::dispose();

				if (hasWindow.exchange(false))
				{
					cv::destroyWindow(_name);
				}
			}
		};

		ADD_CLASS(ShowVideo, waitkey, fullscreen) ;
	}
}
