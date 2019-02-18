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


#include <core/HipeStatus.h>
#include <filter/algos/preprocessing/ComputeRegularGrid.h>

namespace filter {
	namespace algos {
		HipeStatus ComputeRegularGrid::process()
		{
			data::ImageData data = _connexData.pop();
			cv::Mat& image = data.Array()[0];

			if (!image.data) throw HipeException("Error - filter::algos::ComputeRegularGridFilter::process - no input data found.");

			std::vector<cv::Point2f> grid_points = compute_regular_grid(rows, cols, width, height);

			data::ShapeData output;
			output << grid_points;

			PUSH_DATA(output);

			return OK;
		}


		std::vector<cv::Point2f> ComputeRegularGrid::compute_regular_grid(int rows, int cols, float width, float height, bool corners_only)
		{
			/*
			--0----0----0--
			= n * --0--
			= n * ----
			*/
			float x_spacing = width / cols,
				y_spacing = height / rows,
				x_margin = x_spacing / 2,
				y_margin = y_spacing / 2;

			if (corners_only)
			{
				// x_margin is equivalent to "x_min", same for y.
				float x_max = width - x_margin, y_max = height - y_margin;
				return std::vector<cv::Point2f> {
						cv::Point2f{ x_margin, y_margin },
						cv::Point2f{ x_max, y_margin },
						cv::Point2f{ x_margin, y_max },
						cv::Point2f{ x_max, y_max }
				};
			}

			std::vector<cv::Point2f> grid_points(rows * cols);

			for (unsigned int j = 0; j < rows; ++j)
			{
				float y = y_margin + j * y_spacing;
				for (unsigned int i = 0; i < cols; ++i)
				{
					float x = x_margin + i * x_spacing;
					int k = j * cols + i;
					grid_points[k].x = x;
					grid_points[k].y = y;
				}
			}
			return grid_points;
		}
	}
}
