#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>

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
				
				_connexData.push(data::ImageData(output));
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