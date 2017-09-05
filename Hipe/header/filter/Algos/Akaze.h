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
		/**
		 * \brief The Akaze filter will find and match similarities into two images.
		 * It is used to find an object on an image in another one using keypoints and the A-Kaze algorithm
		 * It awaits a PatternData object as input and will output an image containing the computed matching simimarities contoured.
		 */
		class Akaze : public filter::IFilter
		{

			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(Akaze, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(float, inlier_threshold);		//<! The distance threshold used to identify the inliers [TODO]
			REGISTER_P(float, nn_match_ratio);			//<! The ratio used to match the nearests neighbors

			virtual std::string resultAsString() { return std::string("TODO"); };
		public:
			HipeStatus process();
		};

		ADD_CLASS(Akaze, inlier_threshold, nn_match_ratio);
	}
}
