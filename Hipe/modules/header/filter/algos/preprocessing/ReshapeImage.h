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
		class ReshapeImage : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(ReshapeImage, ()), _connexData(data::INDATA)
			{
				target_image_width = 800;
				target_image_height = 800;

				target_grid_width = 4;
				target_grid_height = 4;
			}

			REGISTER_P(int, target_image_width);
			REGISTER_P(int, target_image_height);
			REGISTER_P(int, target_grid_width);
			REGISTER_P(int, target_grid_height);

			HipeStatus process() override;

			cv::Mat reshapeImage(const cv::Mat& image, const std::vector<cv::Vec3f>& circles, const cv::Size& target_size, const cv::Size& expected_grid_size);
		};



		ADD_CLASS(ReshapeImage, target_image_width, target_image_height, target_grid_width, target_grid_height);
	}
}
