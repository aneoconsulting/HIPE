
#include <core/HipeStatus.h>
#include <filter/algos/ComputeRegularGrid.h>
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

			_connexData.push(output);

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