#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <algorithm>

namespace filter
{
	namespace algos
	{
		/**
		 * \var Blur::kernel
		 * The size of the kernel to use (i.e. the number of neighbouring pixels to evaluate).
		 *
		 * \var Blur::type
		 * The type of the blur method to use (mean, median or gaussian).
		 *
		 * \var Blur::sigmaX
		 * The gaussian kernel standard deviation in the X direction. Used only by the Gaussian blur method.
		 *
		 * \var Blur::sigmaY
		 * The gaussian kernel standard deviation in the Y direction. Used only by the Gaussian blur method.
		 */

		 /**
		  * \brief The blur filter will blur an image.
		  * 
		  * The blur method can be normal (mean value of neighbouring pixels), use the median value instead of the mean, or use agaussian kernel. The user must input its desired method.
		  * \see cv::blur()
		  * \see cv::GaussianBlur()
		  * \see cv::medianBlur()
		  */
		class Blur : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Blur, ()), _connexData(data::INOUT)
			{
				kernel = 3;
				type = "default";
				sigmaX = 0.0;
				sigmaY = 0.0;
			}
			REGISTER_P(int, kernel);
			REGISTER_P(std::string, type);
			REGISTER_P(double, sigmaX);
			REGISTER_P(double, sigmaY);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				std::transform(type.begin(), type.end(), type.begin(), ::tolower);

				// Assert kernel size is correct (i.e kernel size is >= 1 and odd)
				if ((kernel < 1) || !(kernel % 2))
				{
					throw HipeException("[Error] Blur::process - blur kernel needs to be odd sized and of value > 1. Passed value is " + std::to_string(kernel));
				}

				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Blur::process - No input data found.");
				}

				if (type == "gaussian")
				{
					cv::GaussianBlur(image, image, cv::Size(kernel, kernel), sigmaX, sigmaY);
				}
				else if (type == "median")
				{
					cv::medianBlur(image, image, kernel);
				}
				else
				{
					cv::blur(image, image, cv::Size(kernel, kernel));
				}

				return OK;
			}
		};
		ADD_CLASS(Blur, kernel, type, sigmaX, sigmaY);
	}
}