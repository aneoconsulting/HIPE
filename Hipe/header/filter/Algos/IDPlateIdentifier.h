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

#include "filter/Algos/IDPlate/IDPlateTools.h"


namespace filter {
	namespace Algos {
		class IDPlateIdentifier : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(IDPlateIdentifier, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				charMinXBound = 0.05;
				charMaxXBound = 0.8;
				charMinFillRatio = 0.2;
				charMaxFillRatio = 0.9;
				charMinWidth = 8;
				charMinHeight = 20;

				dbgMinX = 0.0;
				dbgMaxX = 1.0;
				dbgMinLines = 1;
				ratioY = 1.0;
				ratioHeight = 1.0;
				ratioWidth = 1.0;
				ratioMinArea = 1.0;
				ratioMaxArea = 1.0;
			}
			REGISTER_P(int, _debug);
			REGISTER_P(double, charMinXBound);
			REGISTER_P(double, charMaxXBound);
			REGISTER_P(double, charMinFillRatio);
			REGISTER_P(double, charMaxFillRatio);
			REGISTER_P(int, charMinWidth);
			REGISTER_P(int, charMinHeight);

			REGISTER_P(double, dbgMinX);
			REGISTER_P(double, dbgMaxX);
			REGISTER_P(int, dbgMinLines);
			REGISTER_P(double, ratioY);
			REGISTER_P(double, ratioHeight);
			REGISTER_P(double, ratioWidth);
			REGISTER_P(double, ratioMinArea);
			REGISTER_P(double, ratioMaxArea);

		public:
			HipeStatus process() override;

		private:

			//std::vector<cv::Mat> detectTextArea(const cv::Mat & plateImage, std::vector<cv::Rect>& out_charactersRects);

			/**
			 * \brief Preprocess an ID plate image to facilitate the search of its characters contours.
			 * \param plateImage The color image of the plate
			 * \return Returns a preprocessed binary image where the characters are black and everything else is white
			 */
			cv::Mat preprocessImage(const cv::Mat& plateImage);

			//std::vector<cv::Rect> sortAndFilterCharacters(std::vector<cv::Rect>& characters, int plateImageRows, double lowerBound, double upperBound);
			//std::vector<cv::Rect> blobsRectangle(const cv::Mat& plateImage, std::vector<cv::Rect>& characters);

			//std::vector < std::vector<cv::Rect>> separateCharactersByLines(std::vector<cv::Rect>& charactersSorted, const cv::Mat& debugImage = cv::Mat());
			//void findCharacters(const cv::Mat& plateImage);

			//std::vector<std::vector<cv::Rect>> segmentCharacters(const std::vector<std::vector<cv::Rect>> & lines);

			/**
			 * \brief A Wrapper function for cropping an image around a ROI
			 * \param image The image were the ROIs were extracted from
			 * \param roi The ROIs
			 * \return An image containing only the cropped rect from the source image
			 */
			cv::Mat cropROI(const cv::Mat& image, const cv::Rect& roi);
			/**
			 * \brief Batch process version of cropROI. Crops an image around multiple ROIs
			 * \param image The image on which the ROIs were extracted from
			 * \param rois The ROIs
			 * \return A container filled with all the cropped ROIs from the source image
			 */
			std::vector<cv::Mat> cropROIs(const cv::Mat& image, const std::vector<cv::Rect> & rois);


			/**
			 * \brief Generate an image using another one and write charactersLabels text at charactersRects positions on it.
			 * \param plateImage The source image to use
			 * \param charactersRects The positions of the rects on the image
			 * \param charactersLabels The text to put for each rect
			 * \return An image where the labels in charactersLabels are drawn in their respective charactersRects position
			 */
			cv::Mat createOutputImage(const cv::Mat& plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels);
		};

		ADD_CLASS(IDPlateIdentifier, _debug, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, charMinWidth, charMinHeight, dbgMinX, dbgMaxX, dbgMinLines, ratioY, ratioHeight, ratioWidth, ratioMinArea, ratioMaxArea);

		class LabelOCR {
		public:
			LabelOCR();
			LabelOCR(int debugLevel);
			virtual ~LabelOCR();

			void preProcess(const cv::Mat &InputImage, cv::Mat &binImage);
			std::vector<std::string> runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType);

		private:
			void init();

			std::string runPrediction(const cv::Mat &labelImage, int minConfidence, int imageIndex = -1);
			//void skeletonize(cv::Mat& im);
			//void thinningIteration(cv::Mat& im, int iter);
			//void filterUndesiredChars(std::string &str);

			cv::Mat quantizeImage(const cv::Mat& image, int clusters, int maxIterations = 10);
			cv::Mat binarizeImage(const cv::Mat& image);

			cv::Mat enlargeCharacter(const cv::Mat& character, int margin);

		private:
			cv::Ptr<cv::text::OCRTesseract> tesseOCR_;
			int _debug;
		};


	}
}
