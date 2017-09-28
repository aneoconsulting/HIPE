#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/ImageData.h>
#include <filter/data/OutputData.h>

#include <core/base64.h>
#include <sstream>

namespace filter
{
	namespace algos
	{
		/**
		 * \brief The OutputRawData filter will output the data comming from an image in base64
		 */
		class OutputRawDataEncodedFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(OutputRawDataEncodedFilter, ()), _connexData(data::INOUT)
			{
				type = "";
				quality = -1;
			}

			REGISTER_P(std::string, type);
			REGISTER_P(int, quality);

		public:
			HipeStatus process() override
			{
				filter::data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();

				std::transform(type.begin(), type.end(), type.begin(), ::toupper);
				if (!isTypeKnown(type))
				{
					std::stringstream errorMessage;
					errorMessage << "ERROR - filter::algos::OutputRawDataCompressed: type " << type << " is unknown.";
					throw HipeException(errorMessage.str());
				}

				if (!image.data) throw HipeException("ERROR - filter::algos::OutputRawDataCompressed: no input data");


				std::vector<uchar> imageData;
				cv::imencode(getFileExtensionFromType(type), image, imageData, getQualityFromType(type, quality));

				std::string output = base64_encode(imageData.data(), imageData.size());
				std::string typeValue = filter::data::DataTypeMapper::getStringFromType(data.getType());

				std::cout << "type: " << typeValue << std::endl;
				std::cout << "format: " << type << std::endl;
				std::cout << "compression: " << quality << std::endl;
				std::cout << "width: " << image.cols << std::endl;
				std::cout << "height: " << image.rows << std::endl;
				std::cout << "channels: " << image.channels() << std::endl;
				std::cout << "data: " << output << std::endl;

				return OK;
			}
		private:
			bool isTypeKnown(const std::string & type)
			{
				if (type == "JPG" ||
					type == "PNG")
				{
					return true;
				}
				return false;
			}

			std::vector<int> getQualityFromType(const std::string & type, int quality)
			{
				std::vector<int> params;
				std::string typeErrorMessage;

				if (type == "JPG")
				{
					if (isInRange(quality, 0, 100))
					{
						params.push_back(CV_IMWRITE_JPEG_QUALITY);
						params.push_back(quality);
					}
					else if (quality == -1)
					{
						// Handle default custom parameters
					}
					else
					{
						typeErrorMessage = "\nRange is 0-100";
					}
				}
				else if (type == "PNG")
				{
					if (isInRange(quality, 0, 9))
					{
						params.push_back(CV_IMWRITE_PNG_COMPRESSION);
						params.push_back(quality);
					}
					else if (quality == -1)
					{
						// Handle default custom parameters
					}
					else
					{
						typeErrorMessage = "\nRange is 0-9";
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "ERROR - filter::algos::OutputRawDataCompressed::getQualityFromType: quality " << quality << " for type " << type << " exceeds bounds." << typeErrorMessage;
					throw HipeException(errorMessage.str());
				}

				return params;
			}

			bool isInRange(int value, int lesserBound, int upperBound)
			{
				if (value >= 0 && value <= 100) return true;

				return false;
			}

			std::string getFileExtensionFromType(const std::string & type)
			{
				if (type == "JPG")	return ".jpg";
				if (type == "PNG") return ".png";

				std::stringstream errorMessage;
				errorMessage << "ERROR - filter::algos::OutputRawDataCompressed::getFileExtensionFromType: extension " << type << "is unknown.";
				throw HipeException(errorMessage.str());
			}
		};

		ADD_CLASS(OutputRawDataEncodedFilter, type, quality);
	}
}
