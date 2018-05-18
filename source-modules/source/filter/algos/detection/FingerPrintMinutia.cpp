//@HIPE_LICENSE@
#include <filter/algos/detection/FingerPrintMinutia.h>
#include <data/ImageData.h>

#include <stdio.h>
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
		void FingerPrintMinutia::thinningIteration(Mat& im, int iter)
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
		void FingerPrintMinutia::thinning(Mat& im)
		{
			//debugShow("thinning_input", im, true);
			// Enforce the range tob e in between 0 - 255
			im /= 255;

			//std::cout << im << std::endl;

			Mat prev = Mat::zeros(im.size(), CV_8UC1);
			Mat diff;
			do {
				thinningIteration(im, 0);
				//debugShow("thinning_01", im);
				thinningIteration(im, 1);
				//debugShow("thinning_02", im);

				absdiff(im, prev, diff);
				//debugShow("image_thinned", diff);
				im.copyTo(prev);
				//	imshow("jj", 255*im);
				//waitKey(30);
			} while (countNonZero(diff) > 0);

			im *= 255;
		}

		void FingerPrintMinutia::getDescriptor(Mat const im, Mat& descriptor, vector<KeyPoint>  & keypoints)
		{
			Ptr<Feature2D> orb_descriptor = ORB::create();
			Mat input_binary;

			cvtColor(im, input_binary, cv::COLOR_BGR2GRAY);
			// Compute threshold
			float thresholdValue = threshold(input_binary, input_binary.clone(), 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			// Apply thresholding
			input_binary = input_binary < thresholdValue;
			
			Mat input_thinned = input_binary.clone();
			debugShow("input_binary", input_binary, true);
			debugShow("input_thinned", input_thinned, true);

			thinning(input_thinned);
			//thinning(input_thinned);

			debugShow("image_thinned", input_thinned, true);

			Mat harris_corners, harris_normalised;
			harris_corners = Mat::zeros(input_thinned.size(), CV_32FC1);
			cornerHarris(input_thinned, harris_corners, 2, 3, 0.04, BORDER_DEFAULT);
			normalize(harris_corners, harris_normalised, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

			// Playing around with this threshould could improve performance
			float const threshold = 125.0;

			Mat rescaled;
			convertScaleAbs(harris_normalised, rescaled);
			Mat harris_c(rescaled.rows, rescaled.cols, CV_8UC3);
			Mat in[] = { rescaled, rescaled, rescaled };
			int from_to[] = { 0,0, 1,1, 2,2 };
			mixChannels(in, 3, &harris_c, 1, from_to, 3);
			for (int x = 0; x < harris_normalised.cols; x++) {
				for (int y = 0; y < harris_normalised.rows; y++) {
					if ((int)harris_normalised.at<float>(y, x) > threshold) {
						// Draw or store the keypoint location here, just like you decide. In our case we will store the location of the keypoint
						circle(harris_c, Point(x, y), 5, Scalar(0, 255, 0), 1);
						circle(harris_c, Point(x, y), 1, Scalar(0, 0, 255), 1);
						keypoints.push_back(KeyPoint(x, y, 1));
					}
				}
			}

			debugShow("harric_c", harris_c, true);

			orb_descriptor->compute(input_thinned, keypoints, descriptor);
		}

		HipeStatus FingerPrintMinutia::process()
		{
			while (!_connexData.empty()) // While i've parent data
			{

				data::ImageArrayData array(_connexData.pop());
				data::ImageData imagetest(array.Array()[0]);
				data::ImageData imageref(array.Array()[1]);

				Mat descriptorsInput, descriptorsRef;
				vector<KeyPoint> keypointsInput, keypointsRef;
				getDescriptor(imagetest.getMat(), descriptorsInput, keypointsInput);
				getDescriptor(imageref.getMat(), descriptorsRef, keypointsRef);

				//-- Step 2: Matching descriptor vectors using FLANN matcher
				// Parameters extracted from OpenCV Tutorial: https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_matcher/py_matcher.html#flann-based-matcher
				// See also this post: https://stackoverflow.com/questions/11565255/opencv-flann-with-orb-descriptors
				FlannBasedMatcher matcher = FlannBasedMatcher(new flann::LshIndexParams(6, 12, 1), new flann::SearchParams(50));
				std::vector< DMatch > matches;
				matcher.match(descriptorsInput, descriptorsRef, matches);

				//-- Quick calculation of min distances between keypoints
				double min_dist = std::numeric_limits<double>::max();

				//for (auto i = 0; i < descriptorsInput.rows; i++)
				for (unsigned int i = 0; i < matches.size(); ++i)
				{
					double const dist = matches[i].distance;
					if (dist < min_dist) min_dist = dist;

				}
				// Detect "good" match
				std::vector< DMatch > good_matches;
				//for (auto i = 0; i < descriptorsInput.rows; i++)
				for (int i = 0; i < matches.size(); ++i)
				{
					if (matches[i].distance <= std::max(matchcoeff * min_dist, matchthreshold))
					{
						cerr << matches[i].distance << endl;
						good_matches.push_back(matches[i]);
					}
				}

				// Draw only "good" matches
				Mat img_matches;
				drawMatches(imagetest.getMat(), keypointsInput, imageref.getMat(), keypointsRef,
					good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
					vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

				debugShow("matches", img_matches, true);

				PUSH_DATA(data::ImageData(img_matches));
			}
			return OK;
		}

		void FingerPrintMinutia::debugShow(const std::string& name, const cv::Mat& image, bool deleteWindow)
		{
			cv::imshow(name, image);
			cv::waitKey(0);
			if (deleteWindow) cv::destroyWindow(name);
		}
	}
}
