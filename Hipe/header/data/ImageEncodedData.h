#pragma once
#include <data/IOData.h>
#include <data/ImageData.h>

#include <data/data_export.h>

namespace data
{
	/**
	 * \brief ImageEncodedData is the data type used to handle encoded (as jpg, png, ...) image data. Uses OpenCV.
	 */
	class DATA_EXPORT ImageEncodedData : public IOData<ImageData, ImageEncodedData>
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
				throw HipeException("Cannot copy data to another object. Types doesn't match: (" + data::DataTypeMapper::getStringFromType(getType()) + " vs " + data::DataTypeMapper::getStringFromType(left.getType()));
			if (left.Array_const().size() > 1)
				throw HipeException("Number of images inside the source doesn't correspond to a ImageEncodedData");

			ImageData::copyTo(static_cast<ImageData &>(left));

		}*/


		ImageEncodedData& operator=(const ImageEncodedData& left);

		int getChannelsCount() const;

		int getWidth() const;

		int getHeight() const;

		std::string getCompression() const;

		inline bool empty() const
		{
			return ImageData::empty();

		}
	};
}
