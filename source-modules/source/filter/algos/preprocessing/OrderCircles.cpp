//@HIPE_LICENSE@
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
