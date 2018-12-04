//@HIPE_LICENSE@
#pragma once
#include <corefilter/IFilter.h>
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <vector>
#include <string>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/text.hpp>
#pragma warning(pop)


#include "filter/algos/IDPlate/IDPlateTools.h"


namespace filter
{
	namespace algos
	{
		class LabelOCR2
		{
		public:
			LabelOCR2();
			LabelOCR2(int debugLevel);
			virtual ~LabelOCR2();

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
