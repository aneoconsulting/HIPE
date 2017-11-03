#pragma once
#include <data/IOData.h>

namespace data
{
	/**
	 * \brief PointData is the data type used to handle multiple points. Uses OpenCV.
	 */
	class DATA_EXPORT ShapeData : public IOData <Data, ShapeData>
	{
	protected:
		std::vector<cv::Point2f> _pointsArray;	//<! container of all the points. The data are handled by cv::Point2f objects 
		std::vector<cv::Vec3f> _circlesArray;	//<! container of all the circles. The data are handled by cv::Vec3f objects
		std::vector<cv::Rect> _rectsArray;		//<! container of all the rectangles. The data are handled by cv::Rect objects

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
		ShapeData::ShapeData() : IOData(SHAPE)
		{
			::data::Data::registerInstance(new ShapeData(IOData::_Protection()));
			This()._type = SHAPE;
		}


		/**
		* \brief ShapeData copy constructor
		* \param right The ShapeData to copy data from
		*/
		ShapeData::ShapeData(const ShapeData &right) : IOData(right._type)
		{
			registerInstance(right._This);
			_type = right.This_const()._type;
			_decorate = right._decorate;
		}





		virtual ShapeData::~ShapeData()
		{
			IOData::release();
			_circlesArray.clear();
			_pointsArray.clear();
			_rectsArray.clear();

			if (_This)
			{
				This()._circlesArray.clear();
				This()._pointsArray.clear();
				This()._rectsArray.clear();
			}
		}


		/**
		 * \brief Get the points container
		 * \return Returns a reference to the std::vector<cv::Point2f> object containing the points
		 */
		std::vector<cv::Point2f> & PointsArray();

		/**
		* \brief Get the rects container
		* \return Returns a reference to the std::vector<cv::Rect> object containing the rects
		*/
		std::vector<cv::Rect> & RectsArray();

		/**
		* \brief Get the circles container
		* \return Returns a reference to the std::vector<cv::Vec3f> object containing the points
		*/
		std::vector<cv::Vec3f> & CirclesArray();


		/**
		 * \brief Get the points container (const version)
		 * \return Returns a constant reference to the std::vector<cv::Point2f> object containing the points
		 */
		const std::vector<cv::Point2f> & PointsArray_const() const;

		/**
		* \brief Get the points container (const version)
		* \return Returns a constant reference to the std::vector<cv::Rect> object containing the rects
		*/
		const std::vector<cv::Rect> & RectsArray_const() const;

		/**
		* \brief Get the points container (const version)
		* \return Returns a constant reference to the std::vector<cv::Vec3f> object containing the points
		*/
		const std::vector<cv::Vec3f> & CirclesArray_const() const;


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
		 * \brief
		 * \return Returns true if the object doesn't contain any data
		 */
		inline bool empty() const override
		{
			return (This_const()._pointsArray.empty() && This_const()._circlesArray.empty() && This_const()._rectsArray.empty());
		}

		/**
		 * \todo
		 * \brief ShapeData assignment operator
		 * \param left The ShapeData oject to get the data from
		 * \return A reference to the object
		 */
		ShapeData& operator=(const ShapeData& left);


		/**
		* \brief Copy the data of the object to another one
		* \param left The other object where to copy the data to. Its current data will not be overwritten
		*/
		void copyTo(ShapeData& left) const;
	};
}