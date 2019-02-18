//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

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

				PUSH_DATA(data::ImageData(output));
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
