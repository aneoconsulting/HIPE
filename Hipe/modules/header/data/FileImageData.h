//@HIPE_LICENSE@
#pragma once

#include <data/ImageData.h>
#include <coredata/IODataType.h>
#include <string>
#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#pragma warning(pop)

#include <coredata/data_export.h>


namespace boost {
	namespace filesystem {
	class path;
}
}


namespace data
{
	/**
	 * \brief FileImageData is the data type used to handle an image and additonnal information. Uses OpenCV.
	 */
	class DATA_EXPORT FileImageData : public IOData<ImageData, FileImageData>
	{
		/**
		 * \brief Path to the image
		 */
		std::shared_ptr<boost::filesystem::path> _filePath;

		inline cv::Mat asOutput();
	private:

		/**
		* \brief Default fileImage constructor for private usage only
		*/
		FileImageData();

	public:

		/**
		 * \brief FileImageData copy constructor
		 * \param right the FileImageData to copy
		 */
		FileImageData(const FileImageData& right) : IOData(IODataType::IMGF)
		{
			Data::registerInstance(right._This);
		}

		/**
		 * \brief Constructor with path to image
		 * \param filePath Complete path to the image
		 */
		FileImageData(const std::string& filePath);

		/**
		* \brief Constructor with raw or compressed data of image
		* \param data raw or compressed data in base64 of the image image
		*/
		FileImageData(const std::string& base64Data, const std::string& format, int width, int height, int channels);


		/**
		* \brief Copy the image data of the ImageData object to another one.
		* \param left The object where to copy the data to
		*/
		virtual void copyTo(ImageData& left) const;

		/**
		 * \todo
		 * \brief FileImageData assignment operator
		 * \param left The FileImageData object to get the data from
		 * \return A reference to the object
		 */
		FileImageData& operator=(const FileImageData& left);
	private:
		/**
		 * \brief Get the OpenCV data type corresponding to the image channels count needed to create a cv::Mat object (assuming the data type used is an unsigned char (8U))
		 * \param channels the image channels count
		 * \return the OpenCV value corresponding to a CV_8UCX image where x is the number of channels
		 */
		int getCV8UTypeFromChannels(int channels);
	};
}
