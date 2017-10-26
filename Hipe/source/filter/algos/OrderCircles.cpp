#include <OrderCircles.h>
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
			if (!res.empty()) {
				data::ImageArrayData output;
				output << res;
				_connexData.push(output);
				return OK;
			}
			return EMPTY_RESULT;
		}
	}
}
