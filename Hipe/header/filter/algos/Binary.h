#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
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
		 * \var Binary::type
		 * The thresholding type to apply. The type name is truncated from its "THRESH_" prefix. \see cv::ThresholdTypes
		 *
		 * \var Binary::threshold
		 * The threshold value to decide if a pixel will be black, or white
		 *
		 * \var Binary::otsu
		 * If set to true, the otsu algorithm will be used insted of the inputed threshold value. The otsu algorithm will dinamycally compute the optimal threshold. Mind that the algorithm only works with 8-bit images.
		 *
		 * \var Binary::value
		 * The value parameter is the value a pixel will take if it passes the threshold test
		 */

		/**
		 * \brief The Binary filter will convert a grayscale image to a black and white one.
		 * \see cv::threshold()
		 */
		class Binary : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Binary, ()), _connexData(data::INDATA)
			{
				type = "BINARY";
				otsu = false;
			}
			REGISTER_P(std::string, type);
			REGISTER_P(double, threshold);
			REGISTER_P(bool, otsu);
			REGISTER_P(double, value);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] BilateralFilter::process - No input data found.");
				}

				cv::Mat output;
				int convertedType = convertType(type);
				if (otsu) convertedType = convertedType | cv::THRESH_OTSU;

				double computedThreshold = cv::threshold(image, output, threshold, value, convertedType);

				_connexData.push(data::ImageData(output));
				return OK;
			}

		private:

			/**
			* \brief Converts a thresholding type inputed as a string to its int value.
			* \param type The type the user inputed. The type must be truncated from its "THRESH_" prefix
			* \return Returns the value corresponding to the inputed type
			* \see cv::ThresholdTypes
			*/
			int convertType(std::string & type)
			{
				std::transform(type.begin(), type.end(), type.begin(), ::toupper);
				if (type == "BINARY")
				{
					return cv::THRESH_BINARY;
				}
				else if (type == "BINARY_INV")
				{
					return cv::THRESH_BINARY_INV;
				}
				else if (type == "TRUNC")
				{
					return cv::THRESH_TRUNC;
				}
				else if (type == "TOZERO")
				{
					return cv::THRESH_TOZERO;
				}
				else if (type == "TOZERO_INV")
				{
					return cv::THRESH_TOZERO_INV;
				}
				else
				{
					throw HipeException("[ERROR] Binary::convertType - Unknown threshold type argument: " + type + ".\nThreshold type name must not conatain the \"THRESH_\" prefix (i.e. BINARY for THRESH_BINARY).");
				}
			}
		};
		ADD_CLASS(Binary, type, threshold, otsu, value);
	}
}