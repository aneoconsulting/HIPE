#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageArrayData.h>
#include <data/ImageData.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <cstdio>
#include <iostream>

namespace filter
{
	namespace algos
	{
		class FingerPrintMinutiae : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(FingerPrintMinutiae, ()), _connexData(data::INDATA)
			{
				minHessian = 400;
				matchthreshold = 0.1;
				matchcoeff = 2;
				_debug = 0;

			}

			REGISTER_P(int, minHessian);
			REGISTER_P(double, matchthreshold);
			REGISTER_P(double, matchcoeff);

			REGISTER_P(int, _debug);


		public:
			HipeStatus process() override;

			void thinning(cv::Mat &im);
			void thinningIteration(cv::Mat& im, int iter);

			cv::Mat preprocessFingerprint(const cv::Mat& fingerprintImage);
			std::vector<cv::KeyPoint> computeMinutiae(const cv::Mat& fingerprintImage);

			cv::Mat computeMinutiaeDescriptors(const cv::Mat& fingerprintImage, std::vector<cv::KeyPoint>& minutiae);
			std::vector<cv::DMatch> matchFingerprints(const cv::Mat& refFingerprintImage, const cv::Mat& refFingerprintDescriptors, const cv::Mat& queryFingerprintImage, const cv::Mat& queryFingerprintDescriptors, const std::vector<cv::KeyPoint>& refFingerprintKeypoints = std::vector<cv::KeyPoint>(), const std::vector<cv::KeyPoint>& queryFingerprintKeypoints = std::vector<cv::KeyPoint>(), bool parallelOnly = false);

			cv::Mat drawMatches(const std::vector<cv::DMatch>& matches, const cv::Mat& refFingerprintImage, const std::vector<cv::KeyPoint>& refKeypoints, const cv::Mat& queryFingerprintImage, const std::vector<cv::KeyPoint>& queryKeypoints);

			bool areDescriptorsParallel(const std::vector<cv::Point2f>& refDescriptorsPositions, const std::vector<cv::Point2f>& queryDescriptorsPositions, float threshold = 5.0f);

			void debugShow(const std::string& name, const cv::Mat& image, bool deleteWindow = false);

		};

		ADD_CLASS(FingerPrintMinutiae, minHessian, matchthreshold, matchcoeff, _debug);
	}
}
#pragma once
