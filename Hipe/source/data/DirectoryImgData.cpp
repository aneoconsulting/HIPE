#include <data/DirectoryImgData.h>

namespace data
{
	std::vector<cv::Mat>& DirectoryImgData::images()
	{
		return This()._array;
	}

	cv::Mat DirectoryImgData::image(int index)
	{
		return This()._array[index];
	}
}
