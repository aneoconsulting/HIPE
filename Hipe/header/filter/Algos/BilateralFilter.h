#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <algorithm>

namespace filter
{
	namespace algos
	{
		class BilateralFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(BilateralFilter, ()), _connexData(data::INDATA)
			{
				border = 4;
			}
			REGISTER_P(int, d);
			REGISTER_P(double, color);
			REGISTER_P(double, space);
			REGISTER_P(int, border);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] BilateralFilter::process - No input data found.");
				}

				cv::Mat output;
				cv::bilateralFilter(image, output, d, color, space, border);

				_connexData.push(data::ImageData(output));
				return OK;
			}
		};
		ADD_CLASS(BilateralFilter, d, color, space, border);
	}
}