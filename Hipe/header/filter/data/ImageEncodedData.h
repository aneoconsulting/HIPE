#pragma once
#include <filter/data/IOData.h>
#include <filter/data/ImageData.h>

namespace filter
{
	namespace data
	{
		/**
		 * \brief ImageEncodedData is the data type used to handle encoded (as jpg, png, ...) image data. Uses OpenCV.
		 */
		class ImageEncodedData : public IOData <ImageData, ImageEncodedData>
		{
		public:
			using IOData::IOData;

		protected:
			unsigned int _channels;
			unsigned int _width;
			unsigned int _height;
			std::string _compression;

		protected:
			ImageEncodedData(IOData::_Protection priv) : IOData(IMGENC)
			{

			}

			ImageEncodedData(IODataType dataType) : IOData(dataType)
			{
			}

		public:
			ImageEncodedData() : IOData(IMGENC)
			{
				Data::registerInstance(new ImageEncodedData(IOData::_Protection()));
				This()._type = IMGENC;
				This()._array.resize(1);
				
				This()._channels = 0;
				This()._width = 0;
				This()._height = 0;
				This()._compression = "UNKNOWN";
				
				_channels = 0;
				_width = 0;
				_height = 0;
				_compression = "UNKNOWN";
			}

			/**
			* \brief Constructor with image data (encoded) in a cv::Mat object
			* \param matrix the image encoded data
			*/
			ImageEncodedData(cv::Mat matrix, int rows, int cols, int channels, std::string compression) : IOData(IMGENC)
			{
				Data::registerInstance(new ImageEncodedData(IOData::_Protection()));
				This()._type = IMGENC;
				This()._array.resize(1);
				This()._array[0] = matrix;
				
				This()._channels = channels;
				This()._width = cols;
				This()._height = rows;
				This()._compression = compression;

				_channels = channels;
				_width = cols;
				_height = rows;
				_compression = compression;
			}

			ImageEncodedData(const ImageEncodedData & ref) : IOData(IMGENC)
			{
				Data::registerInstance(ref._This);
				This()._type = ref.This_const()._type;
				_decorate = ref._decorate;
			}

			virtual ~ImageEncodedData()
			{
				IOData::release();
				_array.clear();
				if (_This) This()._array.clear();
			}

			/**
			* \brief Copy the encoded data to another ImageEncodedData object.
			* \param left The object where to copy the data to
			*/
			/*virtual void copyTo(ImageData& left) const
			{
				if (getType() != left.getType())
					throw HipeException("Cannot copy data to another object. Types doesn't match: (" + filter::data::DataTypeMapper::getStringFromType(getType()) + " vs " + filter::data::DataTypeMapper::getStringFromType(left.getType()));
				if (left.Array_const().size() > 1)
					throw HipeException("Number of images inside the source doesn't correspond to a ImageEncodedData");

				ImageData::copyTo(static_cast<ImageData &>(left));

			}*/

			

			ImageEncodedData& operator=(const ImageEncodedData& left)
			{
				Data::registerInstance(left);
				This()._type = left.This_const().getType();
				This()._decorate = left.This_const().getDecorate();
				This()._channels = left.This_const().getChannelsCount();
				This()._width = left.This_const().getWidth();
				This()._height = left.This_const().getHeight();
				This()._compression = left.This_const().getCompression();

				_type = left.This_const().getType();
				_decorate = left.This_const().getDecorate();
				_channels = left.This_const().getChannelsCount();
				_width = left.This_const().getWidth();
				_height = left.This_const().getHeight();
				_compression = getCompression();
				
				return *this;
			}

			int getChannelsCount() const
			{
				return This_const()._channels;
			}
			
			int getWidth() const
			{
				return This_const()._width;
			}
			
			int getHeight() const
			{
				return This_const()._height;
			}
			
			std::string getCompression() const
			{
				return This_const()._compression;
			}
		};
	}
}
