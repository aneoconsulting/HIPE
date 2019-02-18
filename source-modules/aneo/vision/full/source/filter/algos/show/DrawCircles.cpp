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

#include <filter/algos/show/DrawCircles.h>
#include <data/ShapeData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{

		HipeStatus DrawCircles::process()
		{
			while (!_connexData.empty()) // While i've parent data
			{

				data::ImageData images;
				data::ShapeData circles;

				auto input = _connexData.pop();
				if(input.getType()== data::IMGF)
				{
					images = static_cast<data::ImageData>(input).getMat().clone();
				}
				else circles = static_cast<data::ShapeData&>(input);

				data::Data input2 = _connexData.pop();

				if (input2.getType() == data::IMGF)
				{
					 images = static_cast<data::ImageData>(input2).getMat().clone();					 
				}
				else  circles = static_cast<data::ShapeData&>(input2); 
			
				auto const font_scale = factor_font_scale * std::min(images.getMat().rows, images.getMat().cols);
				auto const s = circles.CirclesArray().size();
				cv::Scalar const circle_color(circle_color_r, circle_color_g, circle_color_b);
				cv::Scalar const center_color(center_color_r, center_color_g, center_color_b);
				cv::Scalar const text_color(text_color_r, text_color_g, text_color_b);
			
						
					for (auto i (0UL); i < s ; ++i)
					{

						cv::Point const center(cvRound(circles.CirclesArray()[i][0]), cvRound(circles.CirclesArray()[i][1]));
						auto  const radius = cvRound(circles.CirclesArray()[i][2]);
						// Draw perimeter.
						cv::circle(images.getMat(), center, radius, circle_color, thickness, 8, 0);
						// Draw center.
						circle(images.getMat(), center, center_radius, center_color, -thickness, 8, 0);
						putText(images.getMat(), std::to_string(i), center, CV_FONT_HERSHEY_PLAIN, font_scale, text_color, 2);
					}
					PUSH_DATA(images);
			}
			return OK;
		}
	}
}
