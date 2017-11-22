#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <data/IODataType.h>
#include <data/IOData.h>
#include <data/ListIOData.h>
#include <data/ImageData.h>
#include <data/SquareCrop.h>
#include <data/VideoData.h>

#include <data/data_export.h>

namespace data
{
	/**
	 * \brief PatternData is the data type used to handle an image, information on its regions of interest (\see SquareCrop), and a request image to find on those regions. Uses OpenCV.
	 */
	class DATA_EXPORT PatternData : public VideoData<PatternData>
	{
	public:
		friend  class SquareCrop;

	private:
		SquareCrop _squareCrop;	//<! The list of all the regions of interest. A region on interest is represented by a position (its upper left corner), a width, and a height
		Data _inputSource;		//<! The image where the regions of interest were extracted
		ImageData _requestImg;	//<! The image to compare to the region of interest
		int _endOfSource;		//<! End of video source flag

	protected:
		PatternData(IOData::_Protection priv) : VideoData(PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{

		}

	public:
		PatternData() : VideoData<PatternData>(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(new PatternData(IOData::_Protection()));
			ImageData inputImage;

			This()._inputSource = static_cast<Data>(inputImage);
			newFrame();
		}


		using VideoData::VideoData;
		PatternData(const Data& base) : VideoData(base), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
		}


		/**
		* \brief A copy Constructor accepting an image (ImageData). Overwrites the input source image
		* \param inputImage The image used to overrite the input source one
		*/
		PatternData(ImageData &inputImage) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(new PatternData(IOData::_Protection()));

			This()._inputSource = static_cast<Data>(inputImage);
			newFrame();
		}


		/**
		 * \brief A copy Constructor accepting a list of 2 Data (CROP and SOURCE)
		 * \param left the list of data
		 */
		PatternData(const std::vector<Data>& left);


		/**
		 * \brief Copy constructor for PatternDate copy
		 * \param left antoher PatternData
		 */
		PatternData(const PatternData& left) : VideoData(left.getType()), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(left._This);

			/*This()._squareCrop = left.This_const()._squareCrop;
			This()._requestImg = left.This_const()._requestImg;
			This()._inputSource = left.This_const()._inputSource;*/
		}


		/**
		 * \brief Overloaded ssignment operator used to copy PatternData objects.
		 * \param left The PatternData object to use as a source for the copy
		 * \return Returns a reference to the copied PatternData object
		 */
		virtual PatternData& operator=(const PatternData& left);

		/**
		 * \todo
		 * \brief Overloaded insersion operator used to copy PatternData objects.
		 * \param left The PatternData object to use as a source for the copy
		 * \return Returns a reference to the copied PatternData object
		 */
		PatternData& operator<<(const PatternData& left);

		/**
		 * \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
		 * \param left The ImageData used to overwrite The PatternData's one
		 * \return Returns a reference to the copied PatternData objet
		 */
		PatternData& operator<<(const ImageData& left);

		/**
		 * \brief Check if the source included in the pattern is a a video coming from a file or coming from a streaming input
		 * Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
		 * \return true if the  source is a video or a streaming video
		 */
		static bool isVideoSource(IODataType dataType);

		/**
		* \brief Check if the source included in the pattern is a an image
		* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
		* \return true if the source is an image
		*/
		static bool isImageSource(IODataType dataType);

		/**
		 * \brief Control if the source is an expected entry
		 * \param dataType
		 * \return
		 */
		static bool isInputSource(IODataType dataType);

		/**
		 * \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
		 * Info : At this step the patternData need to have an ImageData source ONLY.
		 * \return ImageData containing the source to challenge to the pattern image
		 */
		ImageData imageRequest() const;

		/**
		* \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
		* Info : At this step the patternData need to have an ImageData source ONLY.
		* \return ImageData containing the source to challenge the pattern image
		*/
		SquareCrop crops() const;

		/**
		* \brief  The function patterns generate an array of cv::Mat with all the crop representing a sub-matrix of the pattern image.
		* \return ImageData containing the source to challenge the pattern image
		*/
		std::vector<cv::Mat> patterns();

		/**
		 * \brief Get the regions of interest
		 * \return The SquareCrop object containing all the regions of interest
		 */
		SquareCrop getSquareCrop() const;

		/**
		 * \brief Copy the data of the object to another one
		 * \param left The PatternData object to overwrite
		 */
		void copyTo(PatternData& left) const;

		/**
		 * \brief
		 * \return [TODO]
		 */
		Data newFrame();

		/**
		 * \brief Does the request image contain data ?
		 * \return Returns true if the request image doesn't contain any data
		 */
		bool empty() const;

		/**
		 * [TODO]
		 * \brief
		 * \param left
		 * \return
		 */
		PatternData& operator=(const Data& left);
	};
}
