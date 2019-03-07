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

#include <string>
#include <algorithm>

#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

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
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);
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
				while (_connexData.size() > 0)
				{
					data::ImageArrayData dataArray = _connexData.pop();

					for (data::ImageData data : dataArray.Array())
					{
						cv::Mat image = data.getMat();
						if (!image.data)
						{
							throw HipeException("[Error] BilateralFilter::process - No input data found.");
						}

						cv::Mat output;
						int convertedType = convertType(type);
						if (otsu) convertedType = convertedType | cv::THRESH_OTSU;

						if (image.channels() == 3 || image.channels() == 4)
							std::cout << "the number of channel has ;ore than one channel" << std::endl;

						cv::threshold(image, output, threshold, value, convertedType);

						PUSH_DATA(data::ImageData(output));
					}
				}
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
