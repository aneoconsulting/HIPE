#include <data/DirectoryImgData.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

namespace data
{
	void DirectoryImgData::loadImagesData()
	{
		std::vector<cv::String> filenames;

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

	std::vector<cv::Mat>& DirectoryImgData::images()
	{
		return This()._array;
	}

	cv::Mat DirectoryImgData::image(int index)
	{
		return This()._array[index];
	}
}
