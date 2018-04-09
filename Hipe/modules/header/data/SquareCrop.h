#pragma once

#pragma warning(push, 0) 
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#pragma warning(pop) 

#include <coredata/IODataType.h>
#include <coredata/IOData.h>
#include <data/ImageData.h>

#include <coredata/data_export.h>


namespace data
{
	/**
	 * \brief SquareCrop is the data type used to handle an image and information on its regions of interest
	 */
	class DATA_EXPORT SquareCrop : public IOData<Data, SquareCrop>
	{
	protected:
		/**
		 * \brief the list of crop represented by Rectangle and where Rectangle is X,Y the coordiante in the _picture and
		 * width,height the dimension of the rectangle
		 */
		std::vector<cv::Rect> _squareCrop;

		/**
		 * \brief The image to find the crops
		 */
		ImageData _picture;

		/**
		 * \brief This is a cache list of submatrix cropped and generated by the list of _squareCrop
		 * it will be populate by crops when the number of submatrix differ from the number crops
		 */
		std::vector<cv::Mat> _cropCache;

		SquareCrop(IOData::_Protection priv) : IOData(IODataType::SQR_CROP)
		{

		}

	public:
		using IOData::IOData;

	public:
		SquareCrop() : IOData(IODataType::SQR_CROP)
		{
			IOData::_Protection priv;
			Data::registerInstance(new SquareCrop(priv));

			This()._type = SQR_CROP;
		}

		/**
		 * \brief
		 * \param picture The image from where the regions of interest come
		 * \param squareCop The list of the regions of interest linked to the \see picture image
		 */
		SquareCrop(ImageData picture, std::vector<cv::Rect> squareCop) : IOData(IODataType::SQR_CROP)
		{
			IOData::_Protection priv;
			Data::registerInstance(new SquareCrop(priv));
			This()._squareCrop = squareCop;
			This()._picture = picture;
			This()._type = SQR_CROP;
		}

		/**
		 * \brief
		 * \param picture The image from where the regions of interest come
		 * \param squareCrop The region of interest (position on x, y then width and height)
		 */
		SquareCrop(ImageData picture, std::vector<int> squareCrop) : IOData(IODataType::SQR_CROP)
		{
			IOData::_Protection priv;
			Data::registerInstance(new SquareCrop(priv));

			This()._picture = picture;
			This()._type = SQR_CROP;

			*(this) << squareCrop; //be carefull Operator<< is a method that undecorate This()
		}

		SquareCrop(const SquareCrop& left) : IOData(left.getType())
		{
			Data::registerInstance(left._This);
		}

		/**
		 * \brief
		 * \return Returns the std::vector<cv::Rect> containing all the regions of interest
		 */
		std::vector<cv::Rect> getSquareCrop() const;

		/**
		 * \brief
		 * \return Returns the ImageData object containing the source image where the regions of interest are located
		 */
		ImageData getPicture() const;

		virtual SquareCrop& operator=(const SquareCrop& left);

		/**
		 * \brief Add one or multiple regions of interest to the object
		 * \param left the ROI(s) position(s) on the image: x, y, width, height
		 * \return A reference to the object
		 */
		IOData& operator<<(const std::vector<int>& left);

		/**
		 * \brief Add one or multiple regions of interest to the object
		 * \param left The list of the ROI(s) to add
		 * \return A reference to the object
		 */
		IOData& operator<<(const std::vector<cv::Rect>& left);

		/**
		 * \brief Add one or multiple regions of interest to the object, and an image. The image will overwrite the one already in the object
		 * \param leftCrop The list of ROI(s) to add. A ROI is defined by two points (upper left and bottom right corners). Uses OpenCV cv::Point
		 * \param leftImage The image from which the ROI(s) come(s). The image will overwrite the one already in the object
		 * \return A reference to the object
		 */
		IOData& addPair(const std::vector<cv::Point>& leftCrop, const ImageData& leftImage);

		/**
		 * \brief Add one or multiple regions of interest to the object.
		 * \param left The list of ROI(s) to add. A ROI is defined by two points (upper left and bottom right corners). Uses OpenCV cv::Point
		 * \return A reference to the object
		 */
		IOData& operator<<(const std::vector<cv::Point>& left);

		/**
		 * \brief Overwrites the current image with a new one
		 * \param left The ImageData object to overwrite the current one with
		 * \return A reference to the object
		 */
		IOData& operator<<(const ImageData& left);

		/**
		* \brief Overwrites the current image with a new one
		* \param left The image to overwrite the current one with
		* \return A reference to the object
		*/
		IOData& operator<<(const cv::Mat left);

		/**
		* \brief  The function patterns generate an array of cv::Mat with
		* all the crop representing a sub-matrix of the pattern image.
		* \return ImageData containing the source to challenge the pattern image
		*/
		std::vector<cv::Mat>& crops(bool forceRefresh = false);
	};
}
