#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>

#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include <data/FileVideoInput.h>

#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>


#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		/**
		 * \var ShowVideo::waitkey
		 * The time a frame should be shown. A null value will let the frame be shown permanently and wait for user input.
		 *
		 * \var ShowVideo::fullscreen
		 * Should the video be played in exclusive fullscreen mode or in a dedicated window?
		 */

		/**
		 *\todo
		 * \brief The ShowVideo filter is used to show a video in a dedicated window.
		 */
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

			std::atomic<bool> hasWindow;		//<! [TODO] Boolean used to keep track of the video's dedicated window


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
						cv::namedWindow(_name, CV_WND_PROP_FULLSCREEN);
						cvSetWindowProperty(_name.c_str(), CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
					}
					else
					{
						cv::namedWindow(_name, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO);
					}
				}

				if (_connexData.size() == 0)
				{
					cv::Mat myImage = cv::Mat::zeros(640, 320, CV_8UC3);
					cv::Scalar color(255, 255, 255);

					cv::putText(myImage, "No INPUT VIDEO", cv::Point(320, 160),
						cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);

					::cv::imshow(_name, myImage);

					if (waitkey >= 0)
						cvWaitKey(waitkey);

					return OK;
				}

				data::ImageArrayData images = _connexData.pop();


				//Resize all images coming from the same parent
				for (auto& myImage : images.Array())
				{
					if (myImage.rows <= 0 || myImage.cols <= 0)
					{
						myImage = cv::Mat::zeros(640, 320, CV_8UC3);
						cv::Scalar color(255,255, 255);

						cv::putText(myImage, "No INPUT VIDEO", cv::Point(320, 160),
							cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);

					}
					::cv::imshow(_name, myImage);

					if (waitkey >= 0)
					{
						int cv_wait_key = cvWaitKey(waitkey);

						if (cv_wait_key == 27 && fullscreen) //Esc 
						{
							if (hasWindow.exchange(false))
							{
								cv::destroyWindow(_name);
								fullscreen = false;
							}
						}
						else if (cv_wait_key == 13 && fullscreen == false) // Enter
						{
							if (hasWindow.exchange(false))
							{
								cv::destroyWindow(_name);
								fullscreen = true;
							}
						}
					}
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

		ADD_CLASS(ShowVideo, waitkey, fullscreen);
	}
}
