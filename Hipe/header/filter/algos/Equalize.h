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
		/**
		 * \brief The Equalize filter will equalize the image histogram resulting of the input image analysis. It will improve the contrast of the image.
		 * \see cv::equalizeHist()
		 */
		class Equalize : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Equalize, ()), _connexData(data::INDATA)
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
				cv::equalizeHist(image, output);
				_connexData.push(data::ImageData(output));
				return OK;
			}

		};
		ADD_CLASS(Equalize, unused);
	}
}