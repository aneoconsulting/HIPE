#include <coredata/IOData.h>
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

	const std::vector<four_points>& ShapeData::QuadrilatereArray_const() const
	{
		const ShapeData &ret = This_const();
		return ret._quadrilatere;
	}

	const std::vector<std::vector<cv::Point2f>>& ShapeData::FreeshapeArray_const() const
	{
		return This_const()._freeshape;
	}

	const std::vector<std::string>& ShapeData::IdsArray_const() const
	{
		return This_const()._ids;
	}

	const std::vector<cv::Scalar>& ShapeData::ColorsArray_const() const
	{
		return This_const()._colors;
	}

	std::vector<four_points>& ShapeData::QuadrilatereArray()
	{
		ShapeData &ret = This();
		return ret._quadrilatere;
	}

	std::vector<std::vector<cv::Point2f>>& ShapeData::FreeshapeArray()
	{
		return This()._freeshape;
	}

	std::vector<std::string>& ShapeData::IdsArray()
	{
		return This()._ids;
	}

	std::vector<cv::Scalar>& ShapeData::ColorsArray()
	{
		return This()._colors;
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

	ShapeData& ShapeData::operator<<(const std::vector<four_points>& quads)
	{
		This()._quadrilatere.insert(This()._quadrilatere.end(), quads.begin(), quads.end());
		return *this;
	}

	ShapeData& ShapeData::add(const std::vector<cv::Point2f>& shapes, const cv::Scalar & color, const std::string& id)
	{
		This()._freeshape.push_back(shapes);

		if (!id.empty())
			This()._ids.push_back(id);

		This()._colors.push_back(color);

		return *this;
	}

	bool ShapeData::empty() const
	{
		return (This_const()._pointsArray.empty() && 
			This_const()._circlesArray.empty() && 
			This_const()._rectsArray.empty() && 
			This_const()._quadrilatere.empty() &&
			This_const()._freeshape.empty());
	}


	ShapeData& ShapeData::operator=(ShapeData& left)
	{
		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}


	void ShapeData::copyTo(ShapeData& left) const
	{
		left.This()._circlesArray.clear();
		left.This()._pointsArray.clear();
		left.This()._rectsArray.clear();
		left.This()._quadrilatere.clear();
		left.This()._freeshape.clear();
		left.This()._ids.clear();

		for (auto circle : CirclesArray_const())
		{
			left.This()._circlesArray.push_back(circle);
		}
		for (auto point : PointsArray_const())
		{
			left.This()._pointsArray.push_back(point);
		}
		for (auto rect : RectsArray_const())
		{
			left.This()._rectsArray.push_back(rect);
		}
		for (auto quad : QuadrilatereArray_const())
		{
			left.This()._quadrilatere.push_back(quad);
		}

		for (auto shape : FreeshapeArray_const())
		{
			left.This()._freeshape.push_back(shape);
		}

		for (auto id : IdsArray_const())
		{
			left.This()._ids.push_back(id);
		}
	}
}
