#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>
#include <string>
#include <opencv/cv.hpp>
#include <boost/filesystem/path.hpp>

namespace filter
{
	namespace data
	{
		class FileImageData : public ImageData
		{
			boost::filesystem::path _filePath;


			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		public:

			FileImageData(const std::string & filePath) : ImageData(IODataType::IMGF)
			{
				_filePath = filePath; 

				cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_COLOR);

				addInputData(mat);
			}

		};
	}
}
