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
#include <data/ShapeData.h>

#include <sstream>

namespace filter
{
	namespace algos
	{
		class DetectCirclesFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ShapeData);

			REGISTER(DetectCirclesFilter, ()), _connexData(data::INDATA)
			{
				min_dist = 0.12;
				min_radius = 0.015;
				max_radius = 0.04;
				factor_blur = 220;
				hough_h_threshold = 40;
				hough_l_threshold = 30;

				strict = false;
			}

			REGISTER_P(double, min_dist);
			REGISTER_P(double, min_radius);
			REGISTER_P(double, max_radius);
			REGISTER_P(int, factor_blur);

			REGISTER_P(int, hough_h_threshold);
			REGISTER_P(int, hough_l_threshold);

			REGISTER_P(bool, strict);

			HipeStatus process() override;
			/*!
			@brief Detect circles in the given image.
			@param image The input image.
			@param out_circles The outputed fond circles.
			@param expected_rows The expected number of rows of circles to find in the image.
			@param expected_cols The expected number of cols of circles to find in the image.
			@return A boolean indicating if the expected number of circles was found.

			Use the Hough transform to detect a 2D array of circles in the image with
			the expected properties such as radius and spacing. The detected circles
			are stored in the `circles` member, with one item for each circle. Each
			item contains the x and y coordinates of the circle's center and its
			radius.
			*/
			bool detect_circles(cv::Mat & image, std::vector<cv::Vec3f>& out_circles, int expected_rows, int expected_cols);
		};

		ADD_CLASS(DetectCirclesFilter, min_dist, min_radius, max_radius, factor_blur, hough_h_threshold, hough_l_threshold, strict);
	}
}
