//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)

#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		/**
		 * \brief The Grayscale filter will convert a color image (3 or 4 channels) to a grayscale one (1 or 2 channels).
		 */
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