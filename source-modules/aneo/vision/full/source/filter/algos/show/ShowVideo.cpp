//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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

	
	data::ImageArrayData images;
	while (_connexData.size() != 0)
	{
		data::Data d_ctx = _connexData.pop();
		
		if (d_ctx.getType() == data::IODataType::IMGF)
		{
			images = d_ctx;
			break;
		}
	}

	

	if (images.getType() != data::IMGF || images.empty())
	{
		cv::Mat myImage = cv::Mat::zeros(640, 320, CV_8UC3);
		if (myImage.rows <= 0 || myImage.cols <= 0)
		{
			myImage = cv::Mat::zeros(640, 320, CV_8UC3);
			cv::Scalar color(255, 255, 255);

			cv::putText(myImage, "No INPUT VIDEO", cv::Point(320, 160),
				cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);
		}
		::cv::imshow(_name, myImage);
		return OK;
	}
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
