#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <opencv2/xfeatures2d.hpp>


namespace filter
{
	namespace algos
	{
		class LatchDebug : public filter::IFilter
		{
			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(LatchDebug, ()), _connexData(data::INDATA)
			{
				hessianThreshold = 100;
				skip_frame = 2;
				compareImagePath = "";
			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);
			REGISTER_P(int, hessianThreshold);
			REGISTER_P(int, skip_frame);
			REGISTER_P(std::string, compareImagePath);

		public:

			HipeStatus process()
			{
				double homography_array[9] = { 7.6285898e-01, -2.9922929e-01, 2.2567123e+02,
					3.3443473e-01, 1.0143901e+00, -7.6999973e+01,
					3.4663091e-04, -1.4364524e-05, 1.0000000e+00 };
				cv::Mat homography = cv::Mat(3, 3, CV_64F, homography_array);

				data::PatternData patternData = _connexData.pop();
				std::vector<cv::Mat> patterns = patternData.patterns();
				if (patterns.empty())
					throw HipeException("There is no Pattern to find in Latch Algorithm");

				//TODO : manage pultiple crop ....ATM only 1 crop is taken in account
				cv::Mat patternImage;
				if (compareImagePath != "")
				{
					patternImage = cv::imread(compareImagePath);
					if(!patternImage.data)
						throw HipeException("[Error] LatchDebug::process - Couldn't read input image from exposed parameter compareImagePath");
				}
				else
				{
					patternImage = patterns[0];
				}

				cv::Mat requestImage = patternData.imageRequest().getMat();

				std::vector<cv::KeyPoint> kpts1, kpts2;
				cv::Mat desc1, desc2;

				cv::Mat mask;

				cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(hessianThreshold);

				detector->detect(patternImage, kpts1, mask);
				detector->detect(requestImage, kpts2, mask);
				cv::Ptr<cv::xfeatures2d::LATCH> latch = cv::xfeatures2d::LATCH::create();
				latch->compute(patternImage, kpts1, desc1);
				//cv::Ptr<cv::Latch> Latch2 = cv::Latch::create();
				latch->compute(requestImage, kpts2, desc2);

				cv::BFMatcher matcher(cv::NORM_HAMMING);
				std::vector<std::vector<cv::DMatch> > nn_matches;
				matcher.knnMatch(desc1, desc2, nn_matches, 2);

				std::vector<cv::KeyPoint> matched1, matched2, inliers1, inliers2;
				std::vector<cv::DMatch> good_matches;
				for (size_t i = 0; i < nn_matches.size(); i++) {
					cv::DMatch first = nn_matches[i][0];
					float dist1 = nn_matches[i][0].distance;
					float dist2 = nn_matches[i][1].distance;

					if (dist1 < nn_match_ratio * dist2) {
						matched1.push_back(kpts1[first.queryIdx]);
						matched2.push_back(kpts2[first.trainIdx]);
					}
				}

				for (unsigned i = 0; i < matched1.size(); i++) {
					cv::Mat col = cv::Mat::ones(3, 1, CV_64F);
					col.at<double>(0) = matched1[i].pt.x;
					col.at<double>(1) = matched1[i].pt.y;

					col = homography * col;
					col /= col.at<double>(2);
					double dist = sqrt(pow(col.at<double>(0) - matched2[i].pt.x, 2) +
						pow(col.at<double>(1) - matched2[i].pt.y, 2));

					if (dist < inlier_threshold) {
						int new_i = static_cast<int>(inliers1.size());
						inliers1.push_back(matched1[i]);
						inliers2.push_back(matched2[i]);
						good_matches.push_back(cv::DMatch(new_i, new_i, 0));
					}
				}

				cv::Mat res;
				cv::drawMatches(patternImage, inliers1, requestImage, inliers2, good_matches, res);

				_connexData.push(data::ImageData(res));
			}
		};

		ADD_CLASS(LatchDebug, inlier_threshold, nn_match_ratio, hessianThreshold, skip_frame, compareImagePath);
	}
}
