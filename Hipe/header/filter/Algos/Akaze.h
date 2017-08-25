#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/ImageData.h>
#include <filter/data/PatternData.h>
#include <opencv2/xfeatures2d.hpp>

namespace filter
{
	namespace algos
	{
		class Akaze : public filter::IFilter
		{

			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(Akaze, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };
		public:
			HipeStatus process();
		};

		ADD_CLASS(Akaze, inlier_threshold, nn_match_ratio);
	}
}
