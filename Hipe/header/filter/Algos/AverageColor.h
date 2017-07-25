#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/core.hpp>


namespace filter
{
	namespace algos
	{
		class AverageColor : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(AverageColor, ()), _connexData(data::INDATA)
			{

			}
			REGISTER_P(char, unused);

		public:
			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat img = data.getMat();

				cv::Scalar averageColor = cv::mean(img);
				cv::Mat output(cv::Size(1, 1), img.type(), averageColor);

				_connexData.push(output);
				return OK;
			}
		};

		ADD_CLASS(AverageColor, unused);
	}
}
