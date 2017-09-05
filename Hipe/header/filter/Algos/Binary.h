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

				double computedThreshold;
				computedThreshold = cv::threshold(image, output, threshold, value, convertedType);

				_connexData.push(data::ImageData(output));
				return OK;
			}

		private:
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