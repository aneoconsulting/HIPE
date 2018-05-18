//@HIPE_LICENSE@
#pragma once
#include <coredata/IOData.h>
#include <vector>
#pragma warning(push, 0) 
#include <opencv2/core/types.hpp>
#pragma warning(pop) 



namespace data
{
	typedef std::vector<cv::Point2f> four_points;
	class DATA_EXPORT ShapeData : public IOData <Data, ShapeData>
	{
	protected:
		std::vector<cv::Point2f> _pointsArray;	//<! container of all the points. The data are handled by cv::Point2f objects 
		std::vector<cv::Vec3f> _circlesArray;	//<! container of all the circles. The data are handled by cv::Vec3f objects
		std::vector<cv::Rect> _rectsArray;		//<! container of all the rectangles. The data are handled by cv::Rect objects
			
		std::vector<four_points> _quadrilatere;		//<! container of all the quadrilateres. The data are handled by cv::vec4f objects
		std::vector<std::vector<cv::Point> > _freeshape;		//<! container of all the free shape. The data are handled by std::vector<cv::Point2f> objects

		std::vector<std::string> _ids;				//<! container of indexed list of text for shape to dispose with it. 
		std::vector<cv::Scalar> _colors;			//<! container of indexed list to _freeShape of color for shape to dispose. 


		ShapeData(IOData::_Protection priv) : IOData(SHAPE)
		{

		}

		ShapeData(data::IODataType type) : IOData(type)
		{

		}

	public:

		/**
		* \brief ShapeData default constructor, the internal IODataType data type will be "SHAPE"
		*/
		ShapeData() : IOData(SHAPE)
		{
			::data::Data::registerInstance(new ShapeData(IOData::_Protection()));
			This()._type = SHAPE;
		}


		/**
		* \brief ShapeData copy constructor
		* \param right The ShapeData to copy data from
		*/
		ShapeData(const ShapeData &right) : IOData(right._type)
		{
			registerInstance(right._This);
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
			_freeshape.clear();
			_ids.clear();
			_colors.clear();

			if (_This)
			{
				This()._circlesArray.clear();
				This()._pointsArray.clear();
				This()._rectsArray.clear();
				This()._quadrilatere.clear();
				This()._freeshape.clear();
				This()._ids.clear();
				This()._colors.clear();
			}
		}

		std::vector<cv::Point2f> & PointsArray();
		std::vector<cv::Rect> & RectsArray();
		std::vector<cv::Vec3f> & CirclesArray();
		std::vector<four_points> & QuadrilatereArray();
		std::vector<std::vector<cv::Point> > & FreeshapeArray();
		std::vector<std::string> & IdsArray();
		std::vector<cv::Scalar> & ColorsArray();

		const std::vector<cv::Point2f> & PointsArray_const() const;
		const std::vector<cv::Rect> & RectsArray_const() const;
		const std::vector<cv::Vec3f> & CirclesArray_const() const;
		const std::vector<four_points> & QuadrilatereArray_const() const;
		const std::vector<std::vector<cv::Point> > & FreeshapeArray_const () const;
		const std::vector<std::string> & IdsArray_const () const;
		const std::vector<cv::Scalar> & ColorsArray_const () const;


		/**
		 * \brief Add a point to the points container.
		 * \param point The point to add
		 * \return Returns a reference to the ShapeData object
		 */
		ShapeData& operator<<(cv::Point2f point);

		/**
		* \brief Add points to the points container.
		* \param points The point to add
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(const std::vector<cv::Point2f>& points);

		/**
		* \brief Add a rect to the rect container.
		* \param rect The rect to add
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(cv::Rect rect);

		/**
		* \brief Add rects to the rect container.
		* \param rects The rects to add
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(const std::vector<cv::Rect>& rects);

		/**
		* \brief Add a circle to the circles container.
		* \param circle The circle to add
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(cv::Vec3f circle);


		/**
		* \brief Add circles to the circles container.
		* \param circles The circles to add
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(std::vector<cv::Vec3f> circles);

		/**
		* \brief Add quads to the quads container. Be mindful that there's no overloaded version to add a simple quad, use this one if you want to.
		* \param quads The quads to add.
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& operator<<(const std::vector<four_points>& quads);

		/**
		* \brief Add a free shape to a list of shapes.
		* \param shapes The shape to add.
		* \param color rectangle's color
		* \param ids The id to the shape to add.
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& add(const std::vector<cv::Point>& shapes, const cv::Scalar & color = cv::Scalar(255, 255, 255), const std::string & id = std::string());

		/**
		* \brief Add a rectanglee to a list of shapes.
		* \param shapes The shape to add.
		* \param color rectangle's color
		* \param ids The id to the shape to add.
		* \return Returns a reference to the ShapeData object
		*/
		ShapeData& add(const cv::Rect & shapes, const cv::Scalar & color = cv::Scalar(255, 255, 255), const std::string& id = std::string());

		/**
		 * \brief
		 * \return Returns true if the object doesn't contain any data
		 */
		bool empty() const override;

		/**
		 * \todo
		 * \brief ShapeData assignment operator
		 * \param left The ShapeData oject to get the data from
		 * \return A reference to the object
		 */
		ShapeData& operator=(ShapeData& left);

		/**
		* \brief Copy the data of the object to another one
		* \param left The other object where to copy the data to. Its current data will be overwritten
		*/
		void copyTo(ShapeData& left) const;
	};
}
