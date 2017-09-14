#include <filter/IFilter.h>
#include <filter/tools/RegisterClass.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/text.hpp>

#include "filter/algos/IDPlate/IDPlateTools.h"


namespace filter
{
	namespace algos
	{
		/**
		 * \brief The IDPlateIdentifier filter will handle the detection of the character of an ID plate then their recognition with machine learning
		 * 
		 * \var IDPlateIdentifier::minXPos
		 * The minimum position on the X Axis to start searching for characters.
		 *
		 * \var IDPlateIdentifier::maxXPos
		 * The maximum position on the X Axis to stop searching for characters.
		 *
		 * \var IDPlateIdentifier::minLines
		 * The minimum number of lines of text we can fit on the image(minimum number of lines means big character).Used to compute the minimum and maximum height a character can have.
		 *
		 * \var IDPlateIdentifier::ratioY
		 * The percentage of the average characters' height. Every characters are not exactly centered on the same line. We use the ratio to search around this line.
		 *
		 * \var IDPlateIdentifier::ratioMinArea
		 * The percentage of the average characters' area. Used to compute the minimum area a character can have to be valid.
		 *
		 * \var IDPlateIdentifier::ratioMaxArea
		 * The percentage of the average characters' area. Used to compute the maximum area a character can have to be valid.
		 *
		 * \var IDPlateIdentifier::_debug
		 * The debug level to use to print informations and show images
		 */
		class IDPlateIdentifier : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(IDPlateIdentifier, ()), _connexData(data::INDATA)
			{
				_debug = 0;

				minXPos = 0.0;
				maxXPos = 1.0;
				minLines = 1;
				ratioY = 1.0;
				ratioMinArea = 1.0;
				ratioMaxArea = 1.0;
			}

			REGISTER_P(int, _debug);


			REGISTER_P(double, minXPos);
			REGISTER_P(double, maxXPos);
			REGISTER_P(int, minLines);
			REGISTER_P(double, ratioY);
			REGISTER_P(double, ratioMinArea);
			REGISTER_P(double, ratioMaxArea);


		public:
			HipeStatus process() override;

		private:

			/**
			 * \brief Preprocess an ID plate image to help the search of its characters contours.
			 * \param plateImage The color image of the plate
			 * \return Returns a preprocessed binary image where the characters are blackon a white background
			 */
			cv::Mat preprocessImage(const cv::Mat& plateImage);

			/**
			 * \brief A Wrapper function for cropping an image around a ROI
			 * \param image The image on which the ROI will be extracted
			 * \param roi The ROI
			 * \return A new image containing only the cropped rect from the source image
			 */
			cv::Mat cropROI(const cv::Mat& image, const cv::Rect& roi);

			/**
			 * \brief Batch process version of cropROI. Crops an image around multiple ROIs
			 * \param image The image on which the ROI will be extracted
			 * \param rois The ROIs
			 * \return A container filled with all the new images containing only the cropped rects from the source image
			 */
			std::vector<cv::Mat> cropROIs(const cv::Mat& image, const std::vector<cv::Rect>& rois);

			/**
			 * \brief From an image containing characters, generate another one with printed charactersLabels text at charactersRects positions.
			 * \param plateImage The source image to use
			 * \param charactersRects The positions of the rects (where to put text) on the image
			 * \param charactersLabels The text to put for each rect
			 * \return An image where the labels in charactersLabels are drawn in their respective charactersRects position
			 */
			cv::Mat createOutputImage(const cv::Mat& plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels);
		};

		ADD_CLASS(IDPlateIdentifier, _debug, minXPos, maxXPos, minLines, ratioY, ratioMinArea, ratioMaxArea);

		class LabelOCR
		{
		public:
			LabelOCR();
			LabelOCR(int debugLevel);
			virtual ~LabelOCR();

			void preProcess(const cv::Mat& InputImage, cv::Mat& binImage, int debugLevel = 0);
			std::vector<std::string> runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType);

		private:
			void init();

			std::string runPrediction(const cv::Mat& labelImage, int minConfidence, int imageIndex = -1);

			cv::Mat quantizeImage(const cv::Mat& image, int clusters, int maxIterations = 10);
			cv::Mat binarizeImage(const cv::Mat& image);

			cv::Mat enlargeCharacter(const cv::Mat& character, int margin);

		private:
			cv::Ptr<cv::text::OCRTesseract> tesseOCR_;
			int _debug;
		};
	}
}
