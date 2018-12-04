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
				history_frames = 500;
				varThreshold = 32;
			}
			cv::Ptr<cv::BackgroundSubtractorMOG2> background_subtractor_mog2;

			REGISTER_P(int, history_frames);
			REGISTER_P(double, varThreshold);

			HipeStatus process() override;

		};

		ADD_CLASS(ExtractBackground, history_frames, varThreshold);


	}
}