//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)
#include <string>

namespace filter
{
	namespace algos
	{
		/**
		 * \var BilateralFilter::d
		 * The diameter of each pixel neighborhood to use
		 *
		 * \var BilateralFilter::color
		 * The standard deviation in the color space to apply
		 *
		 * \var BilateralFilter::space
		 * The standard deviation in the coordinate space (in pixel terms) to apply
		 *
		 * \var BilateralFilter::border
		 * The border mode used to extrapolate pixels outside of the image. \see cv::BorderTypes
		 */

		/**
		 * \brief The BilateralFilter filter will smooth the image with the bilateral filtering method.
		 *  
		 *  The smoothing method usedwill not alter the edges of the shapes.
		 *  Mind that the filtering algorithm is performed on the CPU, each pass takes a certain amount of time.
		 *  \see cv::bilateralFilter()
		 */
		class BilateralFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(BilateralFilter, ()), _connexData(data::INDATA)
			{
				border = cv::BorderTypes::BORDER_DEFAULT;
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

				PUSH_DATA(data::ImageData(output));
				return OK;
			}
		};
		ADD_CLASS(BilateralFilter, d, color, space, border);
	}
}