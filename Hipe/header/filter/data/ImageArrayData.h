#pragma once
#include <filter/data/IOData.h>

namespace filter 
{
	namespace data 
	{
		/**
		 * \brief ImageArrayData is the data type used to handle multiple images. Uses OpenCV.
		 */
		class ImageArrayData : public IOData <Data, ImageArrayData>
		{
		protected:
			std::vector<cv::Mat> _array;	//<! container of all the images data. The data are handled by cv::Mat objects 
			 
			ImageArrayData(data::IODataType type) : IOData(type)
			{
				/*Data::registerInstance(new ImageArrayData());*/
			}

		public:
			using IOData::IOData;

			/**
			 * \brief ImageArrayData default constructor, the internal IODataType data type will be "SEQIMG"
			 */
			ImageArrayData() : IOData(SEQIMG)
			{
			}

			

			/**
			 * \brief ImageArrayData copy constructor
			 * \param right The ImageArrayData to copy data from
			 */
			ImageArrayData(const data::ImageArrayData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
				_array.resize(0);
				_decorate = true;
			}

			virtual ~ImageArrayData()
			{
				IOData::release();
				_array.clear();
			}

			/**
			 * \brief Get the container of the images' data
			 * \return Returns a reference to the std::vector<cv::Mat> object containing the images' data
			 */
			std::vector<cv::Mat> & Array()
			{
				ImageArrayData &ret = This() ;
				return ret._array;
			}

			/**
			 * \brief Get the container of the images' data (const version)
			 * \return Returns a constant reference to the std::vector<cv::Mat> object containing the images' data
			 */
			const std::vector<cv::Mat> & Array_const() const
			{
				const ImageArrayData &ret = This_const();
				return ret._array;
			}


			/**
			 * \brief Add an image to the container.
			 * \param dataMat The image to add
			 * \return Returns a reference to the ImageArrayData object
			 */
			ImageArrayData & operator<<(cv::Mat dataMat)
			{
				This()._array.push_back(dataMat);
				return *this;
			}

			/**
			 * \brief Copy the images' data of the ImageArrayData object to another one
			 * \param left The other object where to copy the data to
			 */
			virtual void copyTo(ImageArrayData& left) const
			{
				for (const cv::Mat & image: Array_const())
				{
					cv::Mat res;
					image.copyTo(res);

					left.This()._array.push_back(res);
				}
			}

			/**
			 * \brief 
			 * \return Returns true if the object doesn't contain any data
			 */
			inline bool empty() const
			{
				return Array_const().empty();
			}

			/**
			 * \todo
			 * \brief ImageArrayData assignment operator
			 * \param left The ImageArrayData oject to get the data from
			 * \return A reference to the object
			 */
			ImageArrayData& operator=(const ImageArrayData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}
		};
	}
}
