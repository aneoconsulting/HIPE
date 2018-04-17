#include <data/DirectoryImgData.h>

#pragma warning(push, 0) 
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "ImageData.h"
#pragma warning(pop) 

namespace data
{
	void DirectoryImgData::loadImagesData()
	{
		cv::glob(This()._directoryPath, filenames);

		for (size_t i = 0; i < filenames.size(); ++i)
		{
			cv::Mat mat = cv::imread(filenames[i]);

			if (mat.empty())
			{
				std::stringstream strbuild;
				strbuild << "Cannot open file : " << filenames[i];
				throw HipeException(strbuild.str());
			}

			cv::putText(mat,
			            removeDirectoryName(filenames[i]),
			            cv::Point(25, 25), // Coordinates
			            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			            1.0, // Scale. 2.0 = 2x bigger
			            cv::Scalar(255, 255, 255), // Color
			            1); // Anti-alias
			This()._array.push_back(mat);
		}
		if (This()._array.empty())
		{
			std::stringstream iss;
			iss << "No file loaded from directory : " << _directoryPath;
			throw HipeException(iss.str());
		}
	}

	void DirectoryImgData::refreshDirectory()
	{
		This().filenames.clear();
		cv::glob(This()._directoryPath, This().filenames);
		This()._idxFile = 0;
	}

	ImageData DirectoryImgData::nextImageFile()
	{
		if (This()._idxFile >= This().filenames.size())
		{
			This()._idxFile = 0;
			return data::ImageData();
		}

		cv::Mat mat = cv::imread(This().filenames[This()._idxFile]);

		This()._idxFile++;

		return ImageData(mat);

	}

	std::vector<cv::Mat>& DirectoryImgData::images()
	{
		return This()._array;
	}

	cv::Mat DirectoryImgData::image(int index)
	{
		return This()._array[index];
	}
}
