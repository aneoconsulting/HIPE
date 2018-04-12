#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \brief The Invert filter will invert the colors of an image.
		 */
		class Invert : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Invert, ()), _connexData(data::INDATA)
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

				cv::Mat output;
				cv::bitwise_not(image, output);
				PUSH_DATA(data::ImageData(output));
				return OK;
			}

		};
		ADD_CLASS(Invert, unused);
	}
}