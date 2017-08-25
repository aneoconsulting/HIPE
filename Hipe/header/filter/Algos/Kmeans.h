#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/ImageArrayData.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"




namespace filter
{
	namespace algos
	{
		class Kmeans : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);
			REGISTER(Kmeans, ()), _connexData(data::INOUT)
			{

			}

			REGISTER_P(int, attempts);
			REGISTER_P(int, clusterCount)

		    virtual std::string resultAsString() { return std::string("TODO"); };
			cv::Mat KmeansItt(cv::Mat myMAt, int clusterCount, int attemps);
		public:
			HipeStatus process();

		};

		ADD_CLASS(Kmeans, clusterCount, attempts);
	}
}