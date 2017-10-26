#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>
#include <data/ImageArrayData.h>
#include "data/ShapeData.h"

namespace filter
{
	namespace algos
	{
		class ComputeRegularGrid : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ShapeData);

			REGISTER(ComputeRegularGrid, ()), _connexData(data::INDATA)
			{
				_rows = 4;
				_cols = 4;
				_width = 800;
				_height = 800;
			}

			REGISTER_P(unsigned int, _rows);
			REGISTER_P(unsigned int, _cols);
			REGISTER_P(float, _width);
			REGISTER_P(float, _height);

			HipeStatus process() override;

			/*!
			@brief Calculate the position of the points along a regular grid.
			@param grid_size The size of the grid (rows * columns).
			@return A vector of Point2f objects representing the points of the regular grid, orderd by row then column.
			*/
			std::vector<cv::Point2f> compute_regular_grid(unsigned int rows, unsigned int cols, float width, float height, bool corners_only = false);
		};

		ADD_CLASS(ComputeRegularGrid, _rows, _cols, _width, _height);
	}
}
