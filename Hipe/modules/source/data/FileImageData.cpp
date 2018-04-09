#include <data/FileImageData.h>
#include <core/base64.h>

#pragma warning(push, 0)
#include <boost/filesystem/path.hpp>
#pragma warning(pop)

namespace data
{
	cv::Mat FileImageData::asOutput()
	{
		return cv::Mat::zeros(0, 0, CV_8UC1);
	}

	/**
	 * \brief Default fileImage constructor for private usage only
	 */
	FileImageData::FileImageData() : IOData(IODataType::IMGF)
	{
		_filePath = std::make_shared<boost::filesystem::path>();
	}


	/**
	* \brief Constructor with path to image
	* \param filePath Complete path to the image
	*/
	FileImageData::FileImageData(const std::string& filePath) : IOData(IODataType::IMGF)
	{
		Data::registerInstance(new FileImageData());
		This()._filePath = std::make_shared<boost::filesystem::path>(filePath);

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
	FileImageData::FileImageData(const std::string& base64Data, const std::string& format, int width, int height, int channels) : IOData(IODataType::IMGF)
	{
		// Decode base64
		const std::string decoded = base64_decode(base64Data);

		// Put data from string in array to match OpenCV required data type
		std::vector<uchar> dataDecoded = std::vector<uchar>(decoded.begin(), decoded.end());

		// Compressed case, nothing to do with raw
		if (format == "JPG" || format == "PNG")
		{
			cv::Mat dataDecodedMat(1, dataDecoded.size(), CV_8UC1, dataDecoded.data());
			dataDecodedMat = cv::imdecode(dataDecodedMat, cv::IMREAD_UNCHANGED);

			width = dataDecodedMat.cols;
			height = dataDecodedMat.rows;
			channels = dataDecodedMat.channels();

			if (!dataDecodedMat.data) throw HipeException("data::FileImageData::FileImageData: Couldn't decode base64 image data. Either data is corrupted, or format (" + format + ") is wrong");

			dataDecoded.clear();
			dataDecoded.insert(dataDecoded.begin(), dataDecodedMat.datastart, dataDecodedMat.dataend);
		}
		else if (format == "RAW")
		{
			// For now nothing more to do with RAW format
		}
		else
		{
			throw HipeException("unknown base64 data compression format");
		}


		// Create cv::Mat object from received image parameters
		cv::Mat image = cv::Mat(height, width, getCV8UTypeFromChannels(channels));

		// Construct FileImageData object
		Data::registerInstance(new FileImageData());
		This()._filePath = std::make_shared<boost::filesystem::path>(format);
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

	void FileImageData::copyTo(ImageData& left) const
	{
		ImageData::copyTo(static_cast<ImageData &>(left));
	}

	FileImageData& FileImageData::operator=(const FileImageData& left)
	{
		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}

	int FileImageData::getCV8UTypeFromChannels(int channels)
	{
		switch (channels)
		{
		case 1:
			return CV_8UC1;
		case 2:
			return CV_8UC2;
		case 3:
			return CV_8UC3;
		case 4:
			return CV_8UC4;
		default:
			std::stringstream errorMessage;
			errorMessage << "ERROR - data::FileImageData: Channels count (" << channels << ") not handled.";
			throw HipeException(errorMessage.str());
		}
	}
}
