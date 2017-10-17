#pragma once
#include <data/IODataType.h>

#include <data/IOData.h>
#include <data/ImageArrayData.h>
#include <data/data_export.h>

namespace data
{
	/**
	 * \brief ImageData is the data type used to handle an image. Uses OpenCV.
	 */
	class DATA_EXPORT ImageData : public IOData<ImageArrayData, ImageData>
	{
	public:
		using IOData::IOData;

	protected:
		ImageData(IOData::_Protection priv) : IOData(IMGF)
		{

		}

		ImageData(IODataType dataType) : IOData(dataType)
		{
			/*Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = dataType;
			This()._array.resize(1);*/
		}

	public:

		/**
		 * \brief Default empty constructor
		 */
		ImageData() : IOData(IMGF)
		{
			Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = IMGF;
			This()._array.resize(1);
		}

		/**
		 * \brief
		 * \param matrix The image's data
		 */
		ImageData(cv::Mat matrix) : IOData(IMGF)
		{
			Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = IMGF;
			This()._array.resize(1);
			This()._array[0] = matrix;
		}

		ImageData(const ImageData & ref) : IOData(IMGF)
		{
			Data::registerInstance(ref._This);
			This()._type = ref.This_const()._type;
			_decorate = ref._decorate;

		}

		virtual ~ImageData()
		{

			IOData::release();
			_array.clear();
		}


		/**
		 * \brief Copy the image data of the ImageData object to another one.
		 * \param left The object where to copy the data to
		 */
		virtual void copyTo(ImageData& left) const;


		/**
		 * \brief Get the image's data
		 * \return Returns a reference to the cv::Mat object containing the image's data
		 */
		inline cv::Mat& getMat();

		/**
		* \brief Get the image's data (const version)
		* \return Returns a constant reference to the cv::Mat object containing the image's data
		*/
		inline const cv::Mat& getMat() const;

		/**
		* \brief
		* \return Returns true if the object doesn't contain any data
		*/
		inline bool empty() const;

		ImageData& operator=(const Data& left);
	};
}
