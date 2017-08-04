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
			}
			REGISTER_P(int, _debug);
			REGISTER_P(double, charMinXBound);
			REGISTER_P(double, charMaxXBound);
			REGISTER_P(double, charMinFillRatio);
			REGISTER_P(double, charMaxFillRatio);
			REGISTER_P(int, charMinWidth);
			REGISTER_P(int, charMinHeight);

		public:
			HipeStatus process() override;

		private:
			std::vector<cv::Mat> detectTextArea(const cv::Mat & plateImage, std::vector<cv::Rect>& out_charactersRects = std::vector<cv::Rect>());
			cv::Mat preprocessImage(const cv::Mat& plateImage);
			std::vector<cv::Rect> sortAndFilterCharacters(std::vector<cv::Rect>& characters, int plateImageRows, double lowerBound, double upperBound);
			//std::vector<cv::Rect> blobsRectangle(const cv::Mat& plateImage, std::vector<cv::Rect>& characters);
			std::vector < std::vector<cv::Rect>> separateCharactersByLines(std::vector<cv::Rect>& charactersSorted, const cv::Mat& debugImage = cv::Mat());

			cv::Mat createOutputImage(const cv::Mat& plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels);
		};

		ADD_CLASS(IDPlateIdentifier, _debug, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, charMinWidth, charMinHeight);

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
			void filterUndesiredChars(std::string &str);

			cv::Mat quantizeImage(const cv::Mat& image, int clusters, int maxIterations = 10);
			cv::Mat binarizeImage(const cv::Mat& image);

			cv::Mat enlargeCharacter(const cv::Mat& character, int margin);

		private:
			cv::Ptr<cv::text::OCRTesseract> tesseOCR_;
			int _debug;
		};


	}
}
