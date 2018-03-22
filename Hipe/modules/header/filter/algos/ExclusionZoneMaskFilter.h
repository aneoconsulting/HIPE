#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	
namespace filter
{
	namespace algos
	{
		class ExclusionZoneMaskFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(ExclusionZoneMaskFilter, ()), _connexData(data::INDATA)
			{
				clahe_clip_limit = 1.0;
				denoise_h = 20;
				denoise_template_window_size = 20;
				denoise_search_window_size = 40;
				clahe_tile_grid_size_x = 4;
				clahe_tile_grid_size_y = 4;
			}

			REGISTER_P(double, clahe_clip_limit);
			REGISTER_P(double, denoise_h);
			REGISTER_P(int, denoise_template_window_size);
			REGISTER_P(int, denoise_search_window_size);
			REGISTER_P(int, clahe_tile_grid_size_x);
			REGISTER_P(int, clahe_tile_grid_size_y);

			HipeStatus process() override;

			/*!
			@brief Compute the exclusion zone mask.
			@param image The image for which to compute the mask (in-situ).
			*/
			void compute_exclusion_zone_mask(cv::Mat & image);
		};

		ADD_CLASS(ExclusionZoneMaskFilter, clahe_clip_limit, denoise_h, denoise_template_window_size, denoise_search_window_size, clahe_tile_grid_size_x, clahe_tile_grid_size_y);
	}
}
