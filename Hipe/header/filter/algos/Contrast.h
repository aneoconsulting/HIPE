#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>

namespace filter
{
	namespace algos
	{
		class Contrast : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Contrast, ()), _connexData(data::INDATA)
			{
				
			}
			REGISTER_P(char, unused);


			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Equalize::process - No input data found.");
				}

				cv::Mat output = image;
				_connexData.push(data::ImageData(output));
				return OK;
			}

		};
		ADD_CLASS(Contrast, unused);
	}
}