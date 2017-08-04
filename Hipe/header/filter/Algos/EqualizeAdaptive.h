#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>

namespace filter
{
	namespace algos
	{
		class EqualizeAdaptive : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(EqualizeAdaptive, ()), _connexData(data::INDATA)
			{
				clip = 2.0;
				kernel = 8;
			}
			REGISTER_P(double, clip);
			REGISTER_P(int, kernel);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Equalize::process - No input data found.");
				}

				cv::Mat output;

				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clip, cv::Size(kernel, kernel));
				clahe->apply(image, output);

				_connexData.push(data::ImageData(output));
				return OK;
			}

		};
		ADD_CLASS(EqualizeAdaptive, clip, kernel);
	}
}