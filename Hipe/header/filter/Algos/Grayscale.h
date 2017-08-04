#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>

namespace filter
{
	namespace algos
	{
		class Grayscale : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Grayscale, ()), _connexData(data::INOUT)
			{

			}
			REGISTER_P(char, unused);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				for (auto& d : data.Array())
				{
					if (d.channels() == 3 || d.channels() == 4)
						cv::cvtColor(d, d, CV_BGR2GRAY);
					else
					{
						std::cout << "[LOG] Grayscale::process - Image with " << d.channels() << " channels found. Can't convert to grayscale." << std::endl;
					}
				}
				return OK;
			}
		};
		ADD_CLASS(Grayscale, unused);
	}
}