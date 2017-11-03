#pragma once
#include <data/IOData.h>
#include <data/ShapeData.h>

namespace data
{

	/**
	* \brief Get the points container
	* \return Returns a reference to the std::vector<cv::Point2f> object containing the points
	*/
	std::vector<cv::Point2f> & ShapeData::PointsArray()
	{
		ShapeData &ret = This();
		return ret._pointsArray;
	}

	/**
	* \brief Get the rects container
	* \return Returns a reference to the std::vector<cv::Rect> object containing the rects
	*/
	std::vector<cv::Rect> & ShapeData::RectsArray()
	{
		ShapeData &ret = This();
		return ret._rectsArray;
	}

	/**
	* \brief Get the circles container
	* \return Returns a reference to the std::vector<cv::Vec3f> object containing the points
	*/
	std::vector<cv::Vec3f> & ShapeData::CirclesArray()
	{
		ShapeData &ret = This();
		return ret._circlesArray;
	}


	/**
	* \brief Get the points container (const version)
	* \return Returns a constant reference to the std::vector<cv::Point2f> object containing the points
	*/
	const std::vector<cv::Point2f> & ShapeData::PointsArray_const() const
	{
		const ShapeData &ret = This_const();
		return ret._pointsArray;
	}

	/**
	* \brief Get the points container (const version)
	* \return Returns a constant reference to the std::vector<cv::Rect> object containing the rects
	*/
	const std::vector<cv::Rect> & ShapeData::RectsArray_const() const
	{
		const ShapeData &ret = This_const();
		return ret._rectsArray;
	}

	/**
	* \brief Get the points container (const version)
	* \return Returns a constant reference to the std::vector<cv::Vec3f> object containing the points
	*/
	const std::vector<cv::Vec3f> & ShapeData::CirclesArray_const() const
	{
		const ShapeData &ret = This_const();
		return ret._circlesArray;
	}


	/**
	* \brief Add a point to the points container.
	* \param point The point to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(cv::Point2f point)
	{
		This()._pointsArray.push_back(point);
		return *this;
	}

	/**
	* \brief Add points to the points container.
	* \param points The point to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(const std::vector<cv::Point2f>& points)
	{
		This()._pointsArray.insert(This()._pointsArray.end(), points.begin(), points.end());
		return *this;
	}

	/**
	* \brief Add a rect to the rect container.
	* \param rect The rect to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(cv::Rect rect)
	{
		This()._rectsArray.push_back(rect);
		return *this;
	}

	/**
	* \brief Add rects to the rect container.
	* \param rects The rects to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(const std::vector<cv::Rect>& rects)
	{
		This()._rectsArray.insert(This()._rectsArray.end(), rects.begin(), rects.end());
		return *this;
	}

	/**
	* \brief Add a circle to the circles container.
	* \param circle The circle to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(cv::Vec3f circle)
	{
		This()._circlesArray.push_back(circle);
		return *this;
	}

	/**
	* \brief Add circles to the circles container.
	* \param circles The circles to add
	* \return Returns a reference to the ShapeData object
	*/
	ShapeData& ShapeData::operator<<(std::vector<cv::Vec3f> circles)
	{
		This()._circlesArray.insert(This()._circlesArray.end(), circles.begin(), circles.end());
		return *this;
	}



	/**
	* \todo
	* \brief ShapeData assignment operator
	* \param left The ShapeData oject to get the data from
	* \return A reference to the object
	*/
	ShapeData& ShapeData::operator=(const ShapeData& left)
	{
		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}


	/**
	* \brief Copy the data of the object to another one
	* \param left The other object where to copy the data to. Its current data will not be overwritten
	*/
	void ShapeData::copyTo(ShapeData& left) const
	{
		left._circlesArray.clear();
		left._pointsArray.clear();
		left._rectsArray.clear();
		for (auto circle : CirclesArray_const())
		{
			left._circlesArray.push_back(circle);
		}
		for (auto point : PointsArray_const())
		{
			left._pointsArray.push_back(point);
		}
		for (auto rect : RectsArray_const())
		{
			left._rectsArray.push_back(rect);
		}
	}
}