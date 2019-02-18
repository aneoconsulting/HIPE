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

#include <filter/algos/extraction/HideCircles.h>
#include <data/ShapeData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{

		HipeStatus HideCircles::process()
		{
			while (!_connexData.empty()) // While i've parent data
			{
				data::ImageData images;
				data::ShapeData circles;

				data::Data input = _connexData.pop();
				if (input.getType() == data::IMGF)
				{
					images = static_cast<data::ImageData>(input);
				}
				else circles = static_cast<data::ShapeData&>(input);

				data::Data input2 = _connexData.pop();
				if (input2.getType() == data::IMGF)
				{
					images = static_cast<data::ImageData>(input2);

				}
				else  circles = static_cast<data::ShapeData&>(input2);


				cv::Mat imageOutput = images.getMat().clone();
				auto s = circles.CirclesArray().size();


				auto width = imageOutput.cols;
				auto height = imageOutput.rows;

				for (auto i(0); i < s; ++i)
				{

					int
						const r = circles.CirclesArray()[i][2] * radius_factor,
						r_inner = r - averaging_width,
						r_outer = r + averaging_width,
						dist_inner = r_inner * r_inner,
						dist_outer = r_outer * r_outer,
						x = circles.CirclesArray()[i][0],
						y = circles.CirclesArray()[i][1],
						x_min = std::max(0, x - r_outer),
						x_max = std::min(width, x + r_outer),
						y_min = std::min(0, y - r_outer),
						y_max = std::max(height, y + r_outer);


					/*
					Collect all pixels on the given perimeter. Restrict the search to the
					bounding box of the outer radius.
					*/
					std::vector<cv::Vec3b> perimeter_pixels;
					for (auto k = x_min; k < x_max; ++k)
					{
						for (auto l = y_min; l < y_max; ++l)
						{
							auto const dx = k - x;
							auto const dy = l - y;
							auto const distance = dx*dx + dy*dy;
							if (distance >= dist_inner && distance <= dist_outer)
							{
								perimeter_pixels.push_back(imageOutput.at<cv::Vec3b>(l, k));
							}
						}
					}

					// Calculate the average color.
					cv::Scalar const mean_color = mean(perimeter_pixels);
					cv::Point2f const center = { circles.CirclesArray()[i][0], circles.CirclesArray()[i][1] };
					cv::circle(imageOutput, center, r_outer, mean_color, -1);
				}


				PUSH_DATA(data::ImageData(imageOutput));

			}
			return OK;
		}
	}
}
