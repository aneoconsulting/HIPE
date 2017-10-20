#pragma once
#include <data/IOData.h>
#include <data/data_export.h>

namespace data
{
	/**
	 * \brief ImageArrayData is the data type used to handle multiple images. Uses OpenCV.
	 */
	class DATA_EXPORT ImageArrayData : public IOData <Data, ImageArrayData>
	{
	protected:
		std::vector<cv::Mat> _array;	//<! container of all the images data. The data are handled by cv::Mat objects 

		ImageArrayData(data::IODataType type) : IOData(type)
		{
			/*Data::registerInstance(new ImageArrayData());*/
		}

	public:

		/**
		 * \brief ImageArrayData default constructor, the internal IODataType data type will be "SEQIMG"
		 */
		ImageArrayData() : IOData(SEQIMG)
		{

		}

		ImageArrayData(const data::Data &right) : IOData(right.getType())
		{
			if (right.getType() != SEQIMG) throw HipeException("ERROR data::ImageArrayData::ImageArrayData - Only Connexdata should call this constructor.");

			Data::registerInstance(right);
			_array.resize(0);
			_decorate = true;
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
		inline std::vector<cv::Mat>& Array();

		/**
		 * \brief Get the container of the images' data (const version)
		 * \return Returns a constant reference to the std::vector<cv::Mat> object containing the images' data
		 */
		inline const std::vector<cv::Mat>& Array_const() const;

		/**
		 * \brief Add an image to the container.
		 * \param dataMat The image to add
		 * \return Returns a reference to the ImageArrayData object
		 */
		ImageArrayData& operator<<(cv::Mat dataMat);

		/**
		 * \brief Copy the images' data of the ImageArrayData object to another one
		 * \param left The other object where to copy the data to
		 */
		virtual void copyTo(ImageArrayData& left) const;

		/**
		 * \brief
		 * \return Returns true if the object doesn't contain any data
		 */
		inline bool empty() const;

		/**
		 * \todo
		 * \brief ImageArrayData assignment operator
		 * \param left The ImageArrayData oject to get the data from
		 * \return A reference to the object
		 */
		ImageArrayData& operator=(const ImageArrayData& left);
	};
}
