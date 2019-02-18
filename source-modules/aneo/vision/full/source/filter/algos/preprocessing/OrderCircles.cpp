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

#include <filter/algos/preprocessing/OrderCircles.h>
#include <data/ShapeData.h>

namespace filter
{
	namespace algo
	{
		HipeStatus OrderCircles::process()
		{

		
			auto data1 = _connexData.pop();
			auto data2 = _connexData.pop();
			std::vector<cv::Vec3f> circles;
			std::vector<cv::Point2f> points;
			if(!data1.CirclesArray().empty())
			{
				circles = data1.CirclesArray();
				points = data2.PointsArray();
			}
			else
			{
				circles = data2.CirclesArray();
				points = data1.PointsArray();
			}
			if( circles.empty() || points.empty())
			{
				throw HipeException("cirles or points is empty please check data 1 and data 2");
			}
			// Order by points.
			for (unsigned int i = 0; i<points.size(); ++i)
			{
				unsigned int closest_index = i;
				unsigned int shortest_dr = UINT_MAX;
				/*
				Increment from i because the closest circle to each point should be
				unique. Previously found circles can therefore be skipped.
				*/
				for (unsigned int j = i; j <circles.size(); ++j)
				{
					int dx = points[i].x - circles[j][0];
					int dy = points[i].y - circles[j][1];
					unsigned int dr = dx*dx + dy*dy;
					if (dr < shortest_dr)
					{
						shortest_dr = dr;
						closest_index = j;
					}
				}
				// Another circle is closer, swap the values.
				if (closest_index != i)
				{
					std::swap(circles[i], circles[closest_index]);
				}
			}
			
			if (!circles.empty()) {
				data::ShapeData output;
				output << circles;
				PUSH_DATA(output);
				return OK;
			}
			return EMPTY_RESULT;
		}
	}
}
