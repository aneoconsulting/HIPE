//@HIPE_LICENSE@
#include <data/SquareCrop.h>

namespace data
{
	std::vector<cv::Rect> SquareCrop::getSquareCrop() const
	{
		return This_const()._squareCrop;
	}

	ImageData SquareCrop::getPicture() const
	{
		return This_const()._picture;
	}

	SquareCrop& SquareCrop::operator=(const SquareCrop& left)
	{
		if (this == &left) return *this;
		IOData::_Protection priv;
		if (!left._This) Data::registerInstance(new SquareCrop(priv));
		else
			Data::registerInstance(left._This);


		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<int>& left)
	{
		if (left.size() % 4 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << left.size() << " isn't a modulo of 4 (2 positions X,Y and 2 size width,height)";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < left.size(); index += 4)
		{
			cv::Rect rect(left[index], left[index + 1], left[index + 2], left[index + 3]);
			This()._squareCrop.push_back(rect);
		}

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<cv::Rect>& left)
	{
		This()._squareCrop = left;

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::addPair(const std::vector<cv::Point>& leftCrop, const ImageData& leftImage)
	{
		if (leftCrop.size() % 2 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << leftCrop.size() << " isn't a modulo of 2";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < leftCrop.size(); index += 2)
		{
			cv::Rect rect(leftCrop[index], leftCrop[index + 1]);
			This()._squareCrop.push_back(rect);
		}

		This()._picture = leftImage;
		This().crops(true);

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<cv::Point>& left)
	{
		if (left.size() % 2 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << left.size() << " isn't a modulo of 2";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < left.size(); index += 2)
		{
			cv::Rect rect(left[index], left[index + 1]);
			This()._squareCrop.push_back(rect);
		}

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const ImageData& left)
	{
		if (left.empty()) throw HipeException("No more Image to add in SquareCrop");

		This()._picture = left;


		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const cv::Mat left)
	{
		if (left.empty()) throw HipeException("No more Image to add in SquareCrop");

		This()._picture = ImageData(left);


		return *this;
	}

	std::vector<cv::Mat>& SquareCrop::crops(bool forceRefresh)
	{
		if (forceRefresh == false &&
			This_const()._cropCache.size() == This_const()._squareCrop.size())
		{
			return This()._cropCache;
		}

		std::vector<cv::Mat> res;

		for (cv::Rect crop : This()._squareCrop)
		{
			res.push_back(This()._picture.getMat()(crop));
		}

		This()._cropCache = res;

		return This()._cropCache;
	}
}
