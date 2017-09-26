#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include <core/base64.h>

namespace filter
{
	namespace data
	{
		/**
		 * \brief FileImageData is the data type used to handle an image and additonnal information. Uses OpenCV.
		 */
		class FileImageData : public IOData<ImageData, FileImageData>
		{
			/**
			 * \brief Path to the image
			 */
			boost::filesystem::path _filePath;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }
		private:
			FileImageData() : IOData(IODataType::IMGF)
			{

			}

		public:

			/**
			 * \brief FileImageData copy constructor
			 * \param right the FileImageData to copy
			 */
			FileImageData(const FileImageData & right) : IOData(IODataType::IMGF)
			{
				Data::registerInstance(right._This);
			}

			/**
			 * \brief Constructor with path to image
			 * \param filePath Complete path to the image
			 */
			FileImageData(const std::string & filePath) : IOData(IODataType::IMGF)
			{
				Data::registerInstance(new FileImageData());
				This()._filePath = filePath;
				This()._type = IMGF;

				cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_COLOR);
				if (mat.empty())
				{
					std::stringstream strbuild;
					strbuild << "Cannot open file : " << filePath;

					throw HipeException(strbuild.str());
				}
				This()._array.push_back(mat);

			}

			/**
			* \brief Constructor with raw data of image
			* \param data raw data in base64 of the image image
			*/
			FileImageData(const std::string & base64Data, const std::string & format, int width, int height, int channels) : IOData(IODataType::IMGF)
			{
				// Decode base64
				const std::string decoded = base64_decode(base64Data);
				std::vector<uchar> dataDecoded;

				// Create cv::Mat object from received image parameters
				cv::Mat image = cv::Mat(height, width, CV_8UC3);

				// Handle different data formats
				// Nothing to do with raw
				if (format == "RAW")
				{
					// Put data from string in array to match OpenCV required data type
					dataDecoded = std::vector<uchar>(decoded.begin(), decoded.end());
				}
				// Uncompressed case
				else if (format == "JPG" || format == "PNG")
				{
					throw HipeException("Compressed images from base64 data not yet handled");
				}
				else
				{
					throw HipeException("unknown base64 data compression format");
				}


				// Construct FileImageData object
				Data::registerInstance(new FileImageData());
				This()._filePath = "RAW";
				This()._type = IMGF;

				// Handle non continuous matrices (will most probably never occur)
				int lwidth = width;
				int lheight = height;
				if (image.isContinuous())
				{
					lwidth *= lheight;
					lheight = 1;
				}

				// Don't forget channels!
				lwidth *= channels;

				// copy data to matrix
				for (int y = 0; y < lheight; ++y)
				{
					uchar* row = image.ptr<uchar>(y);
					for (int x = 0; x < lwidth; ++x)
					{
						row[x] = dataDecoded[y * lwidth + x];
					}
				}

				if (image.empty())
				{
					throw HipeException("Could not create image from base64 data");
				}

				This()._array.push_back(image);
			}

			//FileImageData(const std::string & dataBase64, bool compressed, std::string compressionType) : IOData(IODataType::IMGF)
			//{
			//	// Decode base64
			//	const std::string decoded = base64_decode(dataBase64);

			//	// Extract header infos
			//	const unsigned int headerLength = 4;
			//	unsigned int headerParamIdx = 0;

			//	const int width = decoded[headerParamIdx++ * sizeof(int)];
			//	const int height = decoded[headerParamIdx++ * sizeof(int)];
			//	const int type = decoded[headerParamIdx++ * sizeof(int)];
			//	const int channels = decoded[headerParamIdx++ * sizeof(int)];

			//	// Create image from decoded data
			//	if (compressed)
			//	{
			//		throw HipeException("Compressed images from base64 data not yet handled");
			//	}
			//	// Uncompressed case
			//	else
			//	{
			//		// Put data from string in array to match OpenCV required data type
			//		std::vector<uchar> dataDecoded(decoded.begin() + headerParamIdx * sizeof(int), decoded.end());

			//		// Construct FileImageData object
			//		Data::registerInstance(new FileImageData());
			//		This()._filePath = "RAW";
			//		This()._type = IMGF;

			//		// Create cv::Mat object from received image parameters
			//		cv::Mat image = cv::Mat(height, width, CV_8UC3);

			//		// Handle non continuous matrices (will most probably never occur)
			//		int lwidth = width;
			//		int lheight = height;
			//		if (image.isContinuous())
			//		{
			//			lwidth *= lheight;
			//			lheight = 1;
			//		}

			//		// Don't forget channels!
			//		lwidth *= channels;

			//		// copy data to matrix
			//		for (int y = 0; y < lheight; ++y)
			//		{
			//			uchar* row = image.ptr<uchar>(y);
			//			for (int x = 0; x < lwidth; ++x)
			//			{
			//				row[x] = dataDecoded[y * lwidth + x];
			//			}
			//		}

			//		if (image.empty())
			//		{
			//			throw HipeException("Could not create image from base64 data");
			//		}
			//		This()._array.push_back(image);
			//	}
			//}

			/**
			* \brief Copy the image data of the ImageData object to another one.
			* \param left The object where to copy the data to
			*/
			virtual void copyTo(ImageData& left) const
			{
				ImageData::copyTo(static_cast<ImageData &>(left));

			}

			/**
			 * \todo
			 * \brief FileImageData assignment operator
			 * \param left The FileImageData object to get the data from
			 * \return A reference to the object
			 */
			FileImageData& operator=(const FileImageData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}

		};
	}
}
