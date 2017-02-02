#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>
#include <string>
#include <opencv/cv.hpp>

namespace filter
{
	namespace data
	{
		class FileImageData : ImageData
		{
			std::string _filePath;
			IODataType _type = IODataType::IMG;


			cv::Mat mat;

			cv::Mat asOutput() { return }


		};
	}
}
