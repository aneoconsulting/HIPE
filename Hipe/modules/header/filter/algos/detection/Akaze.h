#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \var Akaze::inlier_threshold
		 * The distance threshold used to identify the inliers.[TODO]
		 *
		 * \var Akaze::nn_match_ratio
		 * The ratio used to match the nearests neighbors.
		 */

		 /**
		  * \todo
		  * \brief The Akaze filter will find and match similarities into two images.
		  * 
		  * The Akaze filter is used to find an object on an image in another one using keypoints and the A-Kaze algorithm.
		  * It awaits a PatternData object as input and will output an image containing the computed matching simimarities contoured.
		  */
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
