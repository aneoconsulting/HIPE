#include <OrderCircles.h>
#include "data/CircleData.h"

namespace filter
{
	namespace algo
	{
		HipeStatus OrderCircles::process()
		{

			return OK; 			
		}
		void OrderCircles::order_circles_by_grid(std::vector<cv::Point2f> const & pointees)
		{
			auto data = _connexData.pop().getListIoData();
			data::CircleData* circles;
			data::PointData* points;
			if(data[0].getType() == data::CIRCLE )
			{
				circles = reinterpret_cast<data::CircleData*>(&data[0]);
			}
			else if (data[1].getType() == data::CIRCLE)
			{
				circles = reinterpret_cast<data::CircleData*>(&data[1]);
			}
			if (data[0].getType() == data::POINT)
			{
				points = reinterpret_cast<data::PointData*>(&data[0]);
			}
			else if (data[1].getType() == data::POINT)
			{
				points = reinterpret_cast<data::PointData*>(&data[1]);
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
			int rows = 1; 
			int cols = circles.size();
			cv::Mat res(rows, cols, CV_32FC3);
			// Assert created matrix is continuous to write its data using the right method
			if (res.isContinuous())
			{
				cols *= rows;
				rows = 1;
			}

			// Don't forget each circle is formed by x,y coordinates plus its radius
			const int circleDataCount = 3;

			// copy data to matrix by indexing it by rows
			for (int y = 0; y < rows; ++y)
			{
				float* row = res.ptr<float>(y);

				for (int x = 0; x < cols; ++x)
				{
					for (int z = 0; z < circleDataCount; ++z)
					{
						row[x] = circles[y * rows + x][z];
					}
				}
			}
			_connexData.push(res);
		}
	}
}
