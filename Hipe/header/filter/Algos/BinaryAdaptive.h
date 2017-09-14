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
		/**
		 * \var BinaryAdaptive::type
		 * The thresholding method to apply. \see cv::ThresholdTypes
		 *
		 * \var BinaryAdaptive::method
		 * The adaptive thresholding algorithm to use. \see cv::AdaptiveThresholdTypes
		 *
		 * \var BinaryAdaptive::block
		 * The kernel's size to use to compute the threshold. It is the number of neighbouring pixels to analyze.
		 *
		 * \var BinaryAdaptive::value
		 * The value parameter is the value a pixel will take if it passes the threshold test.
		 *
		 * \var BinaryAdaptive::c
		 * A constant to substract to the computed mean or weighted mean. [TODO]
		 */

		/**
		 * \brief The BinaryAdaptive filter will convert a grayscale image to a black and white one.
		 * 
		 * Unlike the Binary filter, the BinaryAdaptive filter will divide the image in sub regions and dinamycally compute the optimal threshold value for each of them.
		 * It is useful for images with variations in illumination.
		 * \see cv::adaptiveThreshold()
		 */
		class BinaryAdaptive : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(BinaryAdaptive, ()), _connexData(data::INDATA)
			{
				type = "BINARY";
				method = "MEAN";
				block = 3;
				c = 5;
			}
			REGISTER_P(std::string, type);
			REGISTER_P(std::string, method);
			REGISTER_P(int, block);
			REGISTER_P(double, value);
			REGISTER_P(double, c);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] BinaryAdaptive::process - No input data found.");
				}

				// Assert block size is correct (odd and >= 3)
				if (!block % 2 || block < 3)
					throw HipeException("[ERROR] BinaryAdaptive::process - block size must be odd and >= 3");


				int convertedType = convertType(type);
				int convertedMethod = convertAdaptive(method);

				cv::Mat output;
				cv::adaptiveThreshold(image, output, value, convertedMethod, convertedType, block, c);

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
				else
				{
					throw HipeException("[ERROR] BinaryAdaptive::convertType - Unknown threshold type argument: " + type + ".\nThreshold type name must not conatain the \"THRESH_\" prefix (i.e. BINARY for THRESH_BINARY).");
				}
			}
			/**
			 * \brief Converts a thresholding method inputed as a string to its int value.
			 * \param method The type the user inputed. The type must be truncated from its "ADAPTIVE_THRESH_" prefix
			 * \return Returns the value corresponding to the inputed type.
			 * \see cv::AdaptiveThresholdTypes
			 */
			int convertAdaptive(std::string & method)
			{
				if (method == "MEAN")
				{
					return cv::ADAPTIVE_THRESH_MEAN_C;
				}
				else if (method == "GAUSSIAN")
				{
					return cv::ADAPTIVE_THRESH_GAUSSIAN_C;
				}
				else
				{
					throw HipeException("[ERROR] BinaryAdaptive::convertAdaptive - Unknown method argument: " + method + ".\nMethod name must not conatain the \"ADAPTIVE_THRESH\" prefix and \"_C\" suffic (i.e. MEAN for ADAPTIVE_THRESH_MEAN_C).");
				}
			}
		};
		ADD_CLASS(BinaryAdaptive, type, method, block, value, c);
	}
}