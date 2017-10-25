#pragma once
#include <data/IOData.h>

namespace filter
{
	namespace data
	{
		/**
		 * \brief PointData is the data type used to handle multiple points. Uses OpenCV.
		 */
		class PointData : public IOData <Data, PointData>
		{
		protected:
			std::vector<cv::Point2f> _array;	//<! container of all the points. The data are handled by cv::Point2f objects 

			PointData(IOData::_Protection priv) : IOData(POINTS)
			{

			}

			PointData(data::IODataType type) : IOData(type)
			{

			}

		public:
			using IOData::IOData;

			/**
			 * \brief PointData default constructor, the internal IODataType data type will be "POINTS"
			 */
			PointData() : IOData(POINTS)
			{
				Data::registerInstance(new PointData(IOData::_Protection()));
				This()._type = POINTS;
			}


			/**
			 * \brief PointData copy constructor
			 * \param right The PointData to copy data from
			 */
			PointData(const data::PointData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
				_array.resize(0);
				_decorate = true;
			}

			virtual ~PointData()
			{
				IOData::release();
				_array.clear();
			}

			/**
			 * \brief Get the points container
			 * \return Returns a reference to the std::vector<cv::Point2f> object containing the points
			 */
			std::vector<cv::Point2f> & Array()
			{
				PointData &ret = This();
				return ret._array;
			}

			/**
			 * \brief Get the points container (const version)
			 * \return Returns a constant reference to the std::vector<cv::Point2f> object containing the points
			 */
			const std::vector<cv::Point2f> & Array_const() const
			{
				const PointData &ret = This_const();
				return ret._array;
			}


			/**
			 * \brief Add a point to the container.
			 * \param point The point to add
			 * \return Returns a reference to the PointData object
			 */
			PointData & operator<<(cv::Point2f point)
			{
				This()._array.push_back(point);
				return *this;
			}

			/**
			 * \brief Copy the points of the object to another one
			 * \param left The other object where to copy the data to. Its current data will not be overwritten
			 */
			virtual void copyTo(PointData& left) const
			{
				if (left.getType() != getType())
					throw HipeException("ERROR - data::PointData - cannot copty data. Types mismatch.");

				for (const cv::Point2f & point : Array_const())
				{
					left.This()._array.push_back(point);
				}
			}

			/**
			 * \brief
			 * \return Returns true if the object doesn't contain any data
			 */
			inline bool empty() const override
			{
				return Array_const().empty();
			}

			/**
			 * \todo
			 * \brief PointData assignment operator
			 * \param left The PointData oject to get the data from
			 * \return A reference to the object
			 */
			PointData& operator=(const PointData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}
		};
	}
}
