//@HIPE_LICENSE@
#pragma once

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>


class CVUtils
{
public:
	static cv::Mat writeTextOnImage(const cv::Mat& matchingImage, const std::string text, bool bottom = false)
	{
		cv::Size size_max;

		size_max.width = matchingImage.size().width;
		size_max.height = matchingImage.size().height;

		int baseline = 0;
		double fontscale = 0.8;
		cv::Scalar color(0, 113, 245); //ANEO COLOR
		cv::Size text_size = cv::getTextSize(text, cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontscale, 2, &baseline);
		int size_border = 30;
		text_size.height += size_border; //Add border area

		cv::Point text_pos;
		if (matchingImage.empty())
			text_pos = cv::Point(0, text_size.height - (size_border / 2));
		else
			text_pos = cv::Point(std::max(0, matchingImage.size().width / 2 - text_size.width / 2), text_size.height - size_border / 2);

		size_max.width = size_max.width + (size_max.width >= text_pos.x + text_size.width ? 0 : text_pos.x + text_size.width);
		size_max.height = size_max.height + text_pos.y + text_size.height;

		cv::Mat res = cv::Mat::zeros(size_max, CV_8UC3);


		cv::putText(res, text, text_pos,
			cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontscale, color, 2);

		if (!matchingImage.empty())
		{
			cv::Rect ROI = cv::Rect(0, text_size.height, matchingImage.size().width, matchingImage.size().height);
			matchingImage.copyTo(res(ROI));
		}

		return res;
	}

};
