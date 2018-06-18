#pragma once


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

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class LargestContour : public filter::IFilter
		{
			std::vector<std::vector<cv::Point>> delta_contours;
			int index;

			SET_NAMESPACE("vision/detection")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(LargestContour, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				delta_contours = std::vector<std::vector<cv::Point> >(10);
				index = 0;
				delta_frames = 5;
			}


			REGISTER_P(int, _debug);

			REGISTER_P(int, delta_frames);

			HipeStatus process() override;

			std::vector<cv::Point> convexHullDeltaTime(std::vector<cv::Point> contours);

		};

		ADD_CLASS(LargestContour, _debug, delta_frames);


	}
}