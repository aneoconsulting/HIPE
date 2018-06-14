//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	
#include <opencv2/video/background_segm.hpp>
#include "ExctractSubImage.h"
#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class ExtractBackground : public filter::IFilter
		{
			SET_NAMESPACE("vision/extraction")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(ExtractBackground, ()), _connexData(data::INDATA)
			{
				unused = 1.0;

			}
			cv::Ptr<cv::cuda::BackgroundSubtractorMOG> background_subtractor_mog2;

			REGISTER_P(double, unused);

			HipeStatus process() override;

		};

		ADD_CLASS(ExtractBackground, unused);


	}
}