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
					throw HipeException("[Error] BilateralFilter::process - No input data found.");
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
					throw HipeException("[ERROR] Binary::convertType - Unknown threshold type argument: " + type + ".\nThreshold type name must not conatain the \"THRESH_\" prefix (i.e. BINARY for THRESH_BINARY).");
				}
			}
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