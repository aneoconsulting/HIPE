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


namespace filter {
	namespace Algos {
		class IDPlateIdentifier : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(IDPlateIdentifier, ()), _connexData(data::INDATA)
			{
				debug_ = false;
			}
			REGISTER_P(bool, debug_);

		public:
			HipeStatus process() override;

			bool compareRectsByPosition(const cv::Rect& a, const cv::Rect& b);

		private:
			std::vector<cv::Mat> detectTextArea(const cv::Mat & plateImage, std::vector<cv::Rect>& out_charactersRects = std::vector<cv::Rect>());
			cv::Mat preprocessImage(const cv::Mat& plateImage);
			std::vector<cv::Rect> sortAndFilterCharacters(std::vector<cv::Rect>& characters, int plateImageRows, double lowerBound, double upperBound);
			std::vector<cv::Rect> blobsRectangle(const cv::Mat& plateImage, std::vector<cv::Rect>& characters);
			std::vector < std::vector<cv::Rect>> separateCharactersByLines(std::vector<cv::Rect>& charactersSorted, const cv::Mat& debugImage = cv::Mat());

			std::vector<cv::Rect> findPlateCharacters(const cv::Mat& plateImage, double xMinPos, double xMaxPos, cv::Mat& binarizedImage = cv::Mat());
			cv::Mat applyMorphTransform(const cv::Mat& image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize);

			int showImage(const cv::Mat& image);
			cv::Mat convertColorToGrayscale(const cv::Mat& plateImageColor);
			cv::Mat convertGrayscaleToBGR(const cv::Mat& plateImage);

			cv::Mat createOutputImage(const cv::Mat& plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels);
		};

		class LabelOCR {
		public:
			LabelOCR();
			LabelOCR(bool showImages);
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

			int showImage(const cv::Mat& image);

		private:
			cv::Ptr<cv::text::OCRTesseract> tesseOCR_;
			bool debug_ = true;
		};

		ADD_CLASS(IDPlateIdentifier, debug_);
	}
}
