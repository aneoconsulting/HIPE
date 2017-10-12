#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/OutputData.h>

#include <core/base64.h>
#include <sstream>
#include <data/ImageEncodedData.h>

namespace filter
{
	namespace algos
	{
		/**
		 * \brief The OutputRawData filter will output the data comming from an image in base64
		 */
		class EncodeDataFilter : public filter::IFilter
		{
			enum CompressionTypes : hUInt64
			{
				JPG = 0x4a5047,
				PNG = 0x504e47
			};
			CONNECTOR(data::ImageData, data::ImageEncodedData);

			REGISTER(EncodeDataFilter, ()), _connexData(data::INDATA)
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
				CompressionTypes compressionType = getTypeFromString(type);

				if (!isTypeKnown(compressionType))
				{
					std::stringstream errorMessage;
					errorMessage << "ERROR - filter::algos::OutputRawDataCompressed: type " << type << " is unknown.";
					throw HipeException(errorMessage.str());
				}

				if (!image.data) throw HipeException("ERROR - filter::algos::OutputRawDataCompressed: no input data");


				std::vector<uchar> imageData;
				cv::imencode(getFileExtensionFromType(compressionType), image, imageData, getQualityFromType(compressionType, quality));

				cv::Mat m = cv::Mat(1, imageData.size(), CV_8UC1);
				memcpy(m.data, imageData.data(), imageData.size() * sizeof(uchar));

				data::ImageEncodedData out(m, image.rows, image.cols, image.channels(), type);

				_connexData.push(out);

				return OK;
			}
		private:
			bool isTypeKnown(CompressionTypes type)
			{
				switch (type)
				{
				case JPG:
				case PNG:
					return true;
				default:
					return false;
				}
			}

			std::vector<int> getQualityFromType(CompressionTypes type, int quality)
			{
				std::vector<int> params;
				std::string typeErrorMessage;

				switch (type)
				{
				case JPG:
					if (isInRange(quality, 0, 100))
					{
						params.push_back(CV_IMWRITE_JPEG_QUALITY);
						params.push_back(quality);
					}
					else if (quality == -1)
					{
						// Handle default custom parameters
						// Current default behavior is handled by OpenCV
					}
					else
					{
						typeErrorMessage = "\nRange is 0-100";
					}
					break;
				case PNG:
					if (isInRange(quality, 0, 9))
					{
						params.push_back(CV_IMWRITE_PNG_COMPRESSION);
						params.push_back(quality);
					}
					else if (quality == -1)
					{
						// Handle default custom parameters
						// Current default behavior is handled by OpenCV
					}
					else
					{
						typeErrorMessage = "\nRange is 0-9";
					}
					break;
				default:
					throw HipeException("ERROR - filter::data::EncodeDataFilter::getTypeFromString: Incorrect compression type (" + getStringFromType(type) + ").");
				}

				if (!typeErrorMessage.empty())
				{
					std::stringstream errorMessage;
					errorMessage << "ERROR - filter::algos::OutputRawDataCompressed::getQualityFromType: quality " << quality << " for type " << getStringFromType(type) << " exceeds bounds." << typeErrorMessage;
					throw HipeException(errorMessage.str());
				}

				return params;
			}

			/**
			 * \brief Checks if a value is contained in a certain range
			 * \param value the value to check
			 * \param lesserBound the lower bound
			 * \param upperBound the upper bound
			 * \return returns true if in range, false either
			 */
			bool isInRange(int value, int lesserBound, int upperBound)
			{
				if (value >= lesserBound && value <= upperBound) return true;

				return false;
			}

			/**
			 * \brief Get the file extension corresponding to a compression type
			 * \param type the type to get the extenstion
			 * \return the extension as a std::string matching the compression type in input
			 */
			std::string getFileExtensionFromType(CompressionTypes type)
			{
				switch (type)
				{
				case JPG:
					return ".jpg";
				case PNG:
					return ".png";
				default:
					throw HipeException("ERROR - filter::data::EncodeDataFilter::getTypeFromString: Incorrect compression type (" + getStringFromType(type) + ").");
				}
			}

			/**
			* \brief Get the associated CompressionTypes enum value from its corresponding name in text
			* \param dataTypeString The name of the requested data type
			* \return The data type as a CompressionTypes enum value
			*/
			static CompressionTypes getTypeFromString(const std::string & compressionTypeString)
			{
				//convert string to hexa long value;
				hUInt64 value = ToHex(compressionTypeString);
				CompressionTypes compressionType = static_cast<CompressionTypes>(value);
				if (value < 0L || compressionType < 0L)
				{
					throw HipeException("ERROR - filter::data::EncodeDataFilter::getTypeFromString: Incorrect compression type (" + compressionTypeString + ").");
				}
				return compressionType;
			}

			/**
			* \brief Get the associated CompressionTypes enum name from its corresponding value
			* \param dataType A CompressionTypes enum value
			* \return The name as an std::string object associated with a CompressionTypes enum value
			*/
			static std::string getStringFromType(const CompressionTypes& compressionType)
			{
				//convert string to hexa long value;
				std::string value = ToString(compressionType);

				return value;
			}
		};

		ADD_CLASS(EncodeDataFilter, type, quality);
	}
}



