#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"




namespace filter
{
	namespace algos
	{
		/**
		 * \var Kmeans::attempts
		 * The number of times the kmeans algorithm is executed using different initial labellings. The algorithm will return the labels that yield the best compactness. \see cv::kmeans
		 *
		 * \var Kmeans::clusterCount
		 * The number of clusters (colors) the output image should have.
		 */

		/**
		 * \brief K-means image segmentation. The filter will find color clusters to compress image and reduce the number of used colors.
		 */
		class Kmeans : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);
			REGISTER(Kmeans, ()), _connexData(data::INOUT)
			{

			}

			REGISTER_P(int, attempts);
			REGISTER_P(int, clusterCount);

			virtual std::string resultAsString() { return std::string("TODO"); };
			cv::Mat KmeansItt(cv::Mat myMAt, int clusterCount, int attemps);
		public:
			HipeStatus process();

		};

		ADD_CLASS(Kmeans, clusterCount, attempts);
	}
}