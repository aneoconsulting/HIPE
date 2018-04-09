#include <filter/algos/detection/FingerprintMinutiae.h>
#include <data/ImageData.h>


#include <iostream>
#include <iostream>
#include <algorithm>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


using namespace std;
using namespace cv;
using namespace xfeatures2d;

namespace filter
{
	namespace algos
	{


		// Perform a single thinning iteration, which is repeated until the skeletization is finalized
		void FingerprintMinutiae::thinningIteration(Mat& im, int iter)
		{
			Mat marker = Mat::zeros(im.size(), CV_8UC1);

			//debugShow("thinning_iter_input", im, true);

			//waitKey();
			for (int i = 1; i < im.rows - 1; i++)
			{
				for (int j = 1; j < im.cols - 1; j++)
				{

					uchar p2 = im.at<uchar>(i - 1, j);
					uchar p3 = im.at<uchar>(i - 1, j + 1);
					uchar p4 = im.at<uchar>(i, j + 1);
					uchar p5 = im.at<uchar>(i + 1, j + 1);
					uchar p6 = im.at<uchar>(i + 1, j);
					uchar p7 = im.at<uchar>(i + 1, j - 1);
					uchar p8 = im.at<uchar>(i, j - 1);
					uchar p9 = im.at<uchar>(i - 1, j - 1);

					int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
						(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
						(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
						(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
					int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
					int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
					int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

					if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
						marker.at<uchar>(i, j) = 1;
				}
			}
			im &= ~marker;
		}

		// Function for thinning any given binary image within the range of 0-255. If not you should first make sure that your image has this range preset and configured!
		void FingerprintMinutiae::thinning(Mat& im)
		{
			//debugShow("thinning_input", im, true);
			// Enforce the range tob e in between 0 - 255
			im /= 255;

			//std::cout << im << std::endl;

			Mat prev = Mat::zeros(im.size(), CV_8UC1);
			Mat diff;
			do {
				thinningIteration(im, 0);
				if (_debug > 1) debugShow("thinning_01", im * 255);
				thinningIteration(im, 1);
				if (_debug > 1) debugShow("thinning_02", im * 255);

				absdiff(im, prev, diff);
				if (_debug > 1) debugShow("absdiff", diff * 255);
				im.copyTo(prev);
				//	imshow("jj", 255*im);
				//waitKey(30);
			} while (countNonZero(diff) > 0);

			im *= 255;
		}

		HipeStatus FingerprintMinutiae::process()
		{
			data::ImageArrayData data(_connexData.pop());
			data::ImageData queryImageData(data.Array_const()[0]);

			std::vector<cv::Mat> refImages;
			for (size_t i = 1; i < data.Array_const().size(); ++i)
			{
				refImages.push_back(data.Array_const()[i]);
			}

			// Preprocess query image, done only once
			cv::Mat queryImage = queryImageData.getMat();
			cv::Mat queryImagePreprocessed = preprocessFingerprint(queryImage);

			// Match query image with other inputed images
			for (auto& currentRefImage : refImages)
			{
				// Preprocess current reference image
				cv::Mat currentRefImagePreprocessed = preprocessFingerprint(currentRefImage);

				std::vector<cv::KeyPoint> queryKeypoints, refKeypoints;
				queryKeypoints = computeMinutiae(queryImagePreprocessed);
				refKeypoints = computeMinutiae(currentRefImagePreprocessed);

				cv::Mat queryDescriptors, refDescriptors;
				queryDescriptors = computeMinutiaeDescriptors(queryImagePreprocessed, queryKeypoints);
				refDescriptors = computeMinutiaeDescriptors(currentRefImagePreprocessed, refKeypoints);

				std::vector<cv::DMatch> foundMatches = matchFingerprints(currentRefImagePreprocessed, refDescriptors, queryImagePreprocessed, queryDescriptors, queryKeypoints, refKeypoints, true);

				cv::Mat resultImage = drawMatches(foundMatches, currentRefImage, refKeypoints, queryImage, queryKeypoints);

				if (_debug) debugShow("found matches", resultImage, true);

				_connexData.push(data::ImageData(resultImage));
			}

			return OK;
		}

		void FingerprintMinutiae::debugShow(const std::string& name, const cv::Mat& image, bool deleteWindow)
		{
			cv::imshow(name, image);
			cv::waitKey(0);
			if (deleteWindow) cv::destroyWindow(name);
		}

		cv::Mat FingerprintMinutiae::preprocessFingerprint(const cv::Mat& fingerprintImage)
		{
			cv::Mat preprocessedImage;

			// Convert image to grayscale
			if (fingerprintImage.channels() == 3)
				cv::cvtColor(fingerprintImage, preprocessedImage, cv::COLOR_BGR2GRAY);
			else
				throw HipeException("Error in FingerPrintMinutia filter: Cannot convert image to grayscale. Unknown input type.");

			// Compute threshold value
			float thresholdValue = threshold(preprocessedImage, preprocessedImage.clone(), 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			// Apply thresholding
			preprocessedImage = preprocessedImage < thresholdValue;

			// Thin image
			thinning(preprocessedImage);

			// Debug
			if (_debug) debugShow("input_binary", fingerprintImage, true);
			if (_debug) debugShow("input_thinned", preprocessedImage, true);

			return preprocessedImage;
		}

		std::vector<cv::KeyPoint> FingerprintMinutiae::computeMinutiae(const cv::Mat& fingerprintImage)
		{
			const int neighborhoodSize = 2;
			const int sobelKernelSize = 3;
			const double harrisFreeParameter = 0.04;

			// Compute corners
			cv::Mat harrisCorners = cv::Mat::zeros(fingerprintImage.size(), CV_32FC1);
			cv::cornerHarris(fingerprintImage, harrisCorners, neighborhoodSize, sobelKernelSize, harrisFreeParameter, BORDER_DEFAULT);

			const double lowerBound = 0.0;
			const double upperBound = 255.0;

			// Normalize corners
			cv::Mat normalized;
			cv::normalize(harrisCorners, normalized, lowerBound, upperBound, NORM_MINMAX, CV_32FC1);

			// Playing around with this threshould could improve performance
			const float threshold = 125.0f;

			std::vector<cv::KeyPoint> keypoints;

			for (size_t y = 0; y < normalized.rows; ++y)
			{
				const float* row = normalized.ptr<float>(y);

				for (size_t x = 0; x < normalized.cols; ++x)
				{
					if (row[x] > threshold)
						keypoints.push_back(cv::KeyPoint(x, y, 1));
				}
			}

			// Select relevant corners found with threshold
			if (_debug > 1)
			{
				cv::Scalar pointColor = cv::Scalar(0, 255, 0);
				cv::Scalar circleColor = cv::Scalar(0, 0, 255);
				cv::Mat temp;
				cv::cvtColor(fingerprintImage, temp, CV_GRAY2BGR);

				for (auto& keypoint : keypoints)
				{
					cv::circle(temp, keypoint.pt, 1, pointColor, 1);
					cv::circle(temp, keypoint.pt, 5, circleColor, 1);
				}

				debugShow("found corners", temp, true);
			}

			return keypoints;
		}

		cv::Mat FingerprintMinutiae::computeMinutiaeDescriptors(const cv::Mat& fingerprintImage, std::vector<cv::KeyPoint>& minutiae)
		{
			cv::Mat descriptors;

			//Ptr<Feature2D> orb_descriptor = ORB::create();
			//orb_descriptor->compute(fingerprintImage, minutiae, descriptors);

			cv::Ptr<Feature2D> sift_descriptor = SIFT::create();
			sift_descriptor->compute(fingerprintImage, minutiae, descriptors);

			return descriptors;
		}

		std::vector<cv::DMatch> FingerprintMinutiae::matchFingerprints(const cv::Mat& refFingerprintImage, const cv::Mat& refFingerprintDescriptors, const cv::Mat& queryFingerprintImage, const cv::Mat& queryFingerprintDescriptors, const std::vector<cv::KeyPoint>& refFingerprintKeypoints, const std::vector<cv::KeyPoint>& queryFingerprintKeypoints, bool parallelOnly)
		{
			if (parallelOnly && (refFingerprintKeypoints.empty() || queryFingerprintKeypoints.empty()))
				throw HipeException("Error in FingerprintMinutiae filter: Tried to extract parallel descriptors, but no keypoints found as input");

			// Use this ctr with orb
			//cv::FlannBasedMatcher flannMatcher = cv::FlannBasedMatcher(new flann::LshIndexParams(6, 12, 1), new flann::SearchParams(50));

			// Use this ctr with sift
			cv::FlannBasedMatcher flannMatcher = cv::FlannBasedMatcher();
			std::vector<DMatch> foundMatches, goodMatches;

			flannMatcher.match(refFingerprintDescriptors, queryFingerprintDescriptors, foundMatches);

			//-- Quick calculation of min distances between keypoints
			double minDistance = std::numeric_limits<double>::max();

			for (auto& match : foundMatches)
			{
				const double distance = match.distance;
				if (distance < minDistance) minDistance = distance;
			}

			// Extract good matches
			std::vector<cv::Point2f> queryMatches, refMatches;
			for (auto& match : foundMatches)
				//for(size_t i = 0; i < foundMatches.size(); ++i)
			{
				//cv::DMatch& match = foundMatches[i];

				if (match.distance <= std::max(matchcoeff*  minDistance, matchthreshold))
				{
					std::cout << "LOG - FingerPringMinutia: acceptable match distance found: " << match.distance << std::endl;
					goodMatches.push_back(match);
					if (parallelOnly)
					{
						queryMatches.push_back(queryFingerprintKeypoints[match.queryIdx].pt);
						refMatches.push_back(refFingerprintKeypoints[match.trainIdx].pt);
					}
				}
			}

			if (!parallelOnly)
				return goodMatches;

			else if (areDescriptorsParallel(refMatches, queryMatches))
				return goodMatches;

			else
				return std::vector<DMatch>();
		}

		cv::Mat FingerprintMinutiae::drawMatches(const std::vector<cv::DMatch>& matches, const cv::Mat& refFingerprintImage, const std::vector<cv::KeyPoint>& refKeypoints, const cv::Mat& queryFingerprintImage, const std::vector<cv::KeyPoint>& queryKeypoints)
		{
			cv::Mat output;
			cv::drawMatches(refFingerprintImage, refKeypoints, queryFingerprintImage, queryKeypoints, matches, output, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

			return output;
		}

		bool FingerprintMinutiae::areDescriptorsParallel(const std::vector<cv::Point2f>& refDescriptorsPositions, const std::vector<cv::Point2f>& queryDescriptorsPositions, float threshold)
		{
			if (refDescriptorsPositions.size() != queryDescriptorsPositions.size())
				throw HipeException("Error in FingerprintMinutiae filter: Cannot extract parallel descriptors, reference and query images' descriptors count mismatch.");

			for (size_t i = 0; i < refDescriptorsPositions.size() - 1; ++i)
			{
				const cv::Point& currRefPoint = refDescriptorsPositions[i];
				const cv::Point& nextRefPoint = refDescriptorsPositions[i + 1];
				const cv::Point& currQueryPoint = queryDescriptorsPositions[i];
				const cv::Point& nextQueryPoint = queryDescriptorsPositions[i + 1];

				const double dx0 = currRefPoint.x - currQueryPoint.x;
				const double dy0 = currRefPoint.y - currQueryPoint.y;

				const double dx1 = nextRefPoint.x - nextQueryPoint.x;
				const double dy1 = nextRefPoint.y - nextRefPoint.y;

				if (abs(dx1 - dx0) > threshold || abs(dy1 - dy0) > threshold) return false;
			}

			return true;
		}
	}
}
