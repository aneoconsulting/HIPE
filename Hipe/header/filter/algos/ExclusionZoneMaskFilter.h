#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	// fastNlMeansDenoisingColored
namespace filter
{
	namespace algos
	{
		class ExclusionZoneMaskFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(ExclusionZoneMaskFilter, ()), _connexData(data::INOUT)
			{
			}

			REGISTER_P(char, unused);
			HipeStatus process() override;

			/*!
			@brief Compute the exclusion zone mask.
			@param image The image for which to compute the mask (in-situ).
			*/
			void compute_exclusion_zone_mask(cv::Mat & image);
		};

		ADD_CLASS(ExclusionZoneMaskFilter, unused);
	}
}
