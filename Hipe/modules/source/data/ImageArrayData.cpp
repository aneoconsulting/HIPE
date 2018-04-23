//@HIPE_LICENSE@
#include <data/ImageArrayData.h>

namespace data
{
	std::vector<cv::Mat>& ImageArrayData::Array()
	{
		ImageArrayData& ret = This();
		return ret._array;
	}

	const std::vector<cv::Mat>& ImageArrayData::Array_const() const
	{
		const ImageArrayData& ret = This_const();
		return ret._array;
	}

	ImageArrayData& ImageArrayData::operator<<(cv::Mat dataMat)
	{
		This()._array.push_back(dataMat);
		return *this;
	}

	void ImageArrayData::copyTo(ImageArrayData& left) const
	{
		for (const cv::Mat& image : Array_const())
		{
			cv::Mat res;
			image.copyTo(res);

			left.This()._array.push_back(res);
		}
	}

	bool ImageArrayData::empty() const
	{
		return Array_const().empty();
	}

	ImageArrayData& ImageArrayData::operator=(const ImageArrayData& left)
	{
		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}
}
