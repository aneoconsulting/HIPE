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

#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class Brightness : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Brightness, ()), _connexData(data::INDATA)
			{
				
			}
			REGISTER_P(int, value);


			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Brightness::process - No input data found.");
				}

				/*
				int channels;
				if (image.channels() == 2)	channels = 1;
				else if (image.channels() == 4) channels = 3;
				else channels = image.channels();

				clampValue(value, 0, 100);

				cv::Mat output(image.size(), image.type());

				for (int y = 0; y < image.rows; ++y)
				{
					for (int x = 0; y < image.cols; ++x)
					{
						for (int i = 0; i < channels; ++i)
						{
							image.at<cv::Vec3b>(y, x)[i] = cv::saturate_cast<uchar>(image.at<cv::Vec3b>(y, x)[i] + value);
						}
					}
				}
				*/

				// Average Brightness
				cv::Mat imageHSV;
				cv::cvtColor(image, imageHSV, CV_BGR2HSV);

				std::vector<cv::Mat> HSVChannels;
				cv::split(imageHSV, HSVChannels);

				cv::imshow("image hsv", imageHSV);
				cv::imshow("image h", HSVChannels[0]);
				cv::imshow("image s", HSVChannels[1]);
				cv::imshow("image v", HSVChannels[2]);
				cv::waitKey(0);

				//int averageBrightness = 0;
				//for (int y = 0; y < imageHSV.rows; ++y)
				//{
				//	for (int x = 0; x < imageHSV.cols; ++x)
				//	{
				//		averageBrightness += imageHSV.at<cv::Vec3b>(y, x)[2];
				//	}
				//}

				//averageBrightness /= imageHSV.rows * imageHSV.cols;
				////averageBrightness = averageBrightness * 100 / 255;

				//for (int y = 0; y < imageHSV.rows; ++y)
				//{
				//	for (int x = 0; x < imageHSV.cols; ++x)
				//	{
				//		imageHSV.at <cv::Vec3b>(y, x)[2] = averageBrightness;
				//	}
				//}
				
				cv::Mat output;
				cv::cvtColor(imageHSV, output, CV_HSV2BGR);
				
				PUSH_DATA(data::ImageData(output));
				return OK;
			}



			inline void clampValue(int& out_value, int lowerBound, int upperBound)
			{
				out_value = cv::min(cv::max(out_value, lowerBound), upperBound);
			}

		};
		ADD_CLASS(Brightness, value);
	}
}
