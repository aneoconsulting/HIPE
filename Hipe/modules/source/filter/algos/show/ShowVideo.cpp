#include <filter/algos/show/ShowVideo.h>
#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

HipeStatus filter::algos::ShowVideo::process()
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
	for (cv::Mat myImage : images.Array())
	{
		if (myImage.rows <= 0 || myImage.cols <= 0)
		{
			myImage = cv::Mat::zeros(640, 320, CV_8UC3);
			cv::Scalar color(255, 255, 255);

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
