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
