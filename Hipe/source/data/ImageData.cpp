#include <data/ImageData.h>


namespace data
{
	void ImageData::copyTo(ImageData& left) const
	{
		if (getType() != left.getType())
			throw HipeException("Cannot left argument in a ImageData");
		if (left.Array_const().size() > 1)
			throw HipeException("Number of images inside the source doesn't correspond to a ImageData");

		ImageArrayData::copyTo(static_cast<ImageArrayData &>(left));
	}

	cv::Mat& ImageData::getMat()
	{
		if (Array_const().empty())
			Array().push_back(cv::Mat(0, 0, 0));

		return Array()[0];
	}

	const cv::Mat& ImageData::getMat() const
	{
		return Array_const()[0];
	}

	bool ImageData::empty() const
	{
		if (Array_const().empty()) return true;

		return (Array_const()[0].empty());
	}

	/*ImageData& ImageData::operator=(const Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}

	ImageData& ImageData::operator=(Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}*/
}
