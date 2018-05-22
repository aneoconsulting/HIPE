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
