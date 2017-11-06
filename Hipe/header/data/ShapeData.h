#pragma once
#include <data/IOData.h>

namespace filter
{
	namespace data
	{
		typedef std::array<cv::Point2f, 4> four_points;
	
		/**
		 * \brief PointData is the data type used to handle multiple points. Uses OpenCV.
		 */
		class ShapeData : public IOData <Data, ShapeData>
		{
		protected:
			std::vector<cv::Point2f> _pointsArray;	//<! container of all the points. The data are handled by cv::Point2f objects 
			std::vector<cv::Vec3f> _circlesArray;	//<! container of all the circles. The data are handled by cv::Vec3f objects
			std::vector<cv::Rect> _rectsArray;		//<! container of all the rectangles. The data are handled by cv::Rect objects
			//vector of array of 4 points: _quadrilatere[0][0].x or .y  
			std::vector<four_points> _quadrilatere;		//<! container of all the quadrilateres. The data are handled by cv::vec4f objects

			ShapeData(IOData::_Protection priv) : IOData(SHAPE)
			{

			}

			ShapeData(data::IODataType type) : IOData(type)
			{

			}

		public:
			using IOData::IOData;


			/**
			 * \brief ShapeData default constructor, the internal IODataType data type will be "SHAPE"
			 */
			ShapeData() : IOData(SHAPE)
			{
				Data::registerInstance(new ShapeData(IOData::_Protection()));
				This()._type = SHAPE;
			}


			/**
			 * \brief ShapeData copy constructor
			 * \param right The ShapeData to copy data from
			 */
			ShapeData(const data::ShapeData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
				_type = right.This_const()._type;
				_decorate = right._decorate;
			}

			virtual ~ShapeData()
			{
				IOData::release();
				_circlesArray.clear();
				_pointsArray.clear();
				_rectsArray.clear();
				_quadrilatere.clear();

				if (_This)
				{
					This()._circlesArray.clear();
					This()._pointsArray.clear();
					This()._rectsArray.clear();
					This()._quadrilatere.clear();
				}
			}


			/**
			 * \brief Get the points container
			 * \return Returns a reference to the std::vector<cv::Point2f> object containing the points
			 */
			std::vector<cv::Point2f> & PointsArray()
			{
				ShapeData &ret = This();
				return ret._pointsArray;
			}

			/**
			* \brief Get the rects container
			* \return Returns a reference to the std::vector<cv::Rect> object containing the rects
			*/
			std::vector<cv::Rect> & RectsArray()
			{
				ShapeData &ret = This();
				return ret._rectsArray;
			}
			std::vector<four_points> & QuadrilatereArray()
			{
				ShapeData &ret = This();
				return ret._quadrilatere;
			}

			/**
			* \brief Get the circles container
			* \return Returns a reference to the std::vector<cv::Vec3f> object containing the points
			*/
			std::vector<cv::Vec3f> & CirclesArray()
			{
				ShapeData &ret = This();
				return ret._circlesArray;
			}


			/**
			 * \brief Get the points container (const version)
			 * \return Returns a constant reference to the std::vector<cv::Point2f> object containing the points
			 */
			const std::vector<cv::Point2f> & PointsArray_const() const
			{
				const ShapeData &ret = This_const();
				return ret._pointsArray;
			}

			/**
			* \brief Get the points container (const version)
			* \return Returns a constant reference to the std::vector<cv::Rect> object containing the rects
			*/
			const std::vector<cv::Rect> & RectsArray_const() const
			{
				const ShapeData &ret = This_const();
				return ret._rectsArray;
			}

			/**
			* \brief Get the points container (const version)
			* \return Returns a constant reference to the std::vector<cv::Vec3f> object containing the points
			*/
			const std::vector<cv::Vec3f> & CirclesArray_const() const
			{
				const ShapeData &ret = This_const();
				return ret._circlesArray;
			}
			const std::vector<four_points> & QuadrilatereArray_const() const
			{
				const ShapeData &ret = This_const();
				return ret._quadrilatere;
			}


			/**
			 * \brief Add a point to the points container.
			 * \param point The point to add
			 * \return Returns a reference to the ShapeData object
			 */
			ShapeData& operator<<(cv::Point2f point)
			{
				This()._pointsArray.push_back(point);
				return *this;
			}

			/**
			* \brief Add points to the points container.
			* \param points The point to add
			* \return Returns a reference to the ShapeData object
			*/
			ShapeData& operator<<(const std::vector<cv::Point2f>& points)
			{
				This()._pointsArray.insert(This()._pointsArray.end(), points.begin(), points.end());
				return *this;
			}

			/**
			* \brief Add a rect to the rect container.
			* \param rect The rect to add
			* \return Returns a reference to the ShapeData object
			*/
			ShapeData& operator<<(cv::Rect rect)
			{
				This()._rectsArray.push_back(rect);
				return *this;
			}

			/**
			* \brief Add rects to the rect container.
			* \param rects The rects to add
			* \return Returns a reference to the ShapeData object
			*/
			ShapeData& operator<<(const std::vector<cv::Rect>& rects)
			{
				This()._rectsArray.insert(This()._rectsArray.end(), rects.begin(), rects.end());
				return *this;
			}

			/**
			* \brief Add a circle to the circles container.
			* \param circle The circle to add
			* \return Returns a reference to the ShapeData object
			*/
			ShapeData& operator<<(cv::Vec3f circle)
			{
				This()._circlesArray.push_back(circle);
				return *this;
			}

			ShapeData& operator<<(four_points quadrilatere)
			{
				This()._quadrilatere.push_back(quadrilatere);
				return *this;
			}

			/**
			* \brief Add circles to the circles container.
			* \param circles The circles to add
			* \return Returns a reference to the ShapeData object
			*/
			ShapeData& operator<<(std::vector<cv::Vec3f> circles)
			{
				This()._circlesArray.insert(This()._circlesArray.end(), circles.begin(), circles.end());
				return *this;
			}


			/**
			 * \brief Copy the data of the object to another one
			 * \param left The other object where to copy the data to. Its current data will not be overwritten
			 */
			virtual void copyTo(ShapeData& left) const
			{
				if (left.getType() != getType())
					throw HipeException("ERROR - data::ShapeData - cannot copty data. Types mismatch.");


				for (const cv::Point2f & point : PointsArray_const())
				{
					left.This()._pointsArray.push_back(point);
				}

				for (const cv::Rect & rect : RectsArray_const())
				{
					left.This()._rectsArray.push_back(rect);
				}

				for (const cv::Vec3f & circle : CirclesArray_const())
				{
					left.This()._circlesArray.push_back(circle);
				}

				for (const four_points & quad : QuadrilatereArray_const())
				{
					left.This()._quadrilatere.push_back(quad);
				}
			}

			/**
			 * \brief
			 * \return Returns true if the object doesn't contain any data
			 */
			inline bool empty() const override
			{
				return (This_const()._quadrilatere.empty()&&This_const()._pointsArray.empty() && This_const()._circlesArray.empty() && This_const()._rectsArray.empty());
			}

			/**
			 * \todo
			 * \brief ShapeData assignment operator
			 * \param left The ShapeData oject to get the data from
			 * \return A reference to the object
			 */
			ShapeData& operator=(const ShapeData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}
		};
	}
}
