#include <MatcherData.h>

namespace data {
	std::vector<cv::DMatch> & MatcherData::goodMatches()
	{
		MatcherData &ret = This();
		return ret._goodMatches;
	}

	std::vector<cv::KeyPoint>& MatcherData::inliers1()
	{
		MatcherData &ret = This();
		return ret._inliers1;
	}

	std::vector<cv::KeyPoint>& MatcherData::inliers2()
	{
		MatcherData &ret = This();
		return ret._inliers2;
	}

	cv::Mat& MatcherData::patternImage()
	{
		MatcherData &ret = This();
		return ret._patternImage;
	}
	cv::Mat& MatcherData::requestImage()
	{
		MatcherData &ret = This();
		return ret._requestImage;
	}


	const std::vector<cv::DMatch> & MatcherData::goodMatches_const() const
	{
		const MatcherData &ret = This_const();
		return ret._goodMatches;
	}

	const std::vector<cv::KeyPoint>& MatcherData::inliers1_const() const
	{
		const MatcherData &ret = This_const();
		return ret._inliers1;
	}

	const std::vector<cv::KeyPoint>& MatcherData::inliers2_const() const
	{
		const MatcherData &ret = This_const();
		return ret._inliers2;
	}

	const cv::Mat& MatcherData::patternImage_const() const
	{
		const MatcherData &ret = This_const();
		return ret._patternImage;
	}
	const cv::Mat& MatcherData::requestImage_const() const
	{
		const MatcherData &ret = This_const();
		return ret._requestImage;
	}

	/**
	* \brief Set the good match array of the object
	*/
	void MatcherData::setGoodMatches(const std::vector<cv::DMatch>& goodMatches)
	{
		MatcherData &ret = This();
		_goodMatches = goodMatches;
		ret._goodMatches = goodMatches;
	}
	/**
	* \brief Set the inliers1 (pattern image inliers2_const) of the object
	*/
	void MatcherData::setInliers1(const std::vector<cv::KeyPoint>& patternImageInliers)
	{
		MatcherData &ret = This();
		_inliers1 = patternImageInliers;
		ret._inliers1 = patternImageInliers;
	}
	/**
	* \brief Set the inliers2 (request image inliers2_const) of the object
	*/
	void MatcherData::setInliers2(const std::vector<cv::KeyPoint>& requestImageInliers)
	{
		MatcherData &ret = This();
		_inliers2 = requestImageInliers;
		ret._inliers1 = requestImageInliers;
	}
	/**
	* \brief Set the pattern image of the object
	*/
	void MatcherData::setPatternImage(const cv::Mat& patternImage)
	{
		MatcherData &ret = This();
		_patternImage = patternImage;
		ret._patternImage = patternImage;
	}
	/**
	* \brief Set the request image of the object
	*/
	void MatcherData::setRequestImage(const cv::Mat& requestImage)
	{
		MatcherData &ret = This();
		_requestImage = requestImage;
		ret._requestImage = requestImage;
	}


	/*MatcherData& operator<<(MatcherData md)
	{
	This()._inliers1.push_back(md.inliers1());
	return *this;
	}*/



	void MatcherData::copyTo(MatcherData& left) const
	{
		if (left.getType() != getType())
			throw HipeException("ERROR - data::MatcherData - cannot copty data. Types mismatch.");


		for (auto matches : goodMatches_const())
		{
			left.This()._goodMatches.push_back(matches);
		}

		for (auto inlier1 : inliers1_const())
		{
			left.This()._inliers1.push_back(inlier1);
		}

		for (auto inlier2 : inliers2_const())
		{
			left.This()._inliers2.push_back(inlier2);
		}
		for (auto inlier2 : inliers2_const())
		{
			left.This()._inliers2.push_back(inlier2);
		}
		left.This()._patternImage.copyTo(patternImage_const());
		left.This()._requestImage.copyTo(requestImage_const());
	}

	/**
	* \brief
	* \return Returns true if the object doesn't contain any data
	*/
	bool MatcherData::empty() const
	{
		return (This_const()._inliers1.empty() && This_const()._inliers2.empty() && This_const()._goodMatches.empty() && This_const()._patternImage.empty() && This_const()._requestImage.empty());
	}

	/**
	* \todo
	* \brief ShapeData assignment operator
	* \param left The ShapeData oject to get the data from
	* \return A reference to the object
	*/
	MatcherData& MatcherData::operator=(const MatcherData& left)
	{
		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}
}