#pragma once

#include "filter/algos/FingerPrint.h"
#include "data/ImageData.h"

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include "algorithm"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"



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

			// Enforce the range tob e in between 0 - 255
			im /= 255;



			Mat prev = Mat::zeros(im.size(), CV_8UC1);
			Mat diff;

			do {
				thinningIteration(im, 0);
				thinningIteration(im, 1);
				absdiff(im, prev, diff);
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

			cvtColor(im, input_binary, cv::COLOR_RGB2GRAY);
			//im.copyTo(input_binary);
			//Mat input_thinned= input_binary < 200;
			//Apply thinning algorithm
			//Mat input_binary;
			threshold(input_binary, input_binary, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			Mat input_thinned = input_binary.clone();
			thinning(input_thinned);
			//thinning(input_thinned);



			Mat harris_corners, harris_normalised;
			harris_corners = Mat::zeros(input_thinned.size(), CV_32FC1);
			cornerHarris(input_thinned, harris_corners, 2, 3, 0.04, BORDER_DEFAULT);
			normalize(harris_corners, harris_normalised, 0, 255, NORM_MINMAX, CV_32FC1, Mat());


			float const threshold = 125.0;

			Mat rescaled;
			convertScaleAbs(harris_normalised, rescaled);
			Mat harris_c(rescaled.rows, rescaled.cols, CV_8UC3);
			Mat in[] = { rescaled, rescaled, rescaled };
			int from_to[] = { 0,0, 1,1, 2,2 };
			mixChannels(in, 3, &harris_c, 1, from_to, 3);
			for (int x = 0; x<harris_normalised.cols; x++) {
				for (int y = 0; y<harris_normalised.rows; y++) {
					if ((int)harris_normalised.at<float>(y, x) > threshold) {
						// Draw or store the keypoint location here, just like you decide. In our case we will store the location of the keypoint
						circle(harris_c, Point(x, y), 5, Scalar(0, 255, 0), 1);
						circle(harris_c, Point(x, y), 1, Scalar(0, 0, 255), 1);
						keypoints.push_back(KeyPoint(x, y, 1));
					}
				}
			}


			orb_descriptor->compute(input_thinned, keypoints, descriptor);
		}

		HipeStatus FingerPrintMinutia::process()
		{
			while (!_connexData.empty()) // While i've parent data
			{

				data::ImageArrayData array(_connexData.pop());
				data::ImageData imagetest(array.Array()[0]);
				data::ImageData imageref(array.Array()[1]);

				Ptr<Feature2D> orb_descriptor = ORB::create();
				Mat descriptorstest, descriptorsref;
				vector<KeyPoint> keypointstest, keypointsref;
				getDescriptor(imagetest.getMat(), descriptorstest, keypointstest);
				getDescriptor(imageref.getMat(), descriptorsref, keypointsref);


				//-- Step 2: Matching descriptor vectors using FLANN matcher
				FlannBasedMatcher matcher;
				std::vector< DMatch > matches;
				matcher.match(descriptorstest, descriptorsref, matches);



				//-- Quick calculation of min distances between keypoints
				double min_dist = 1;

				for (auto i = 0; i < descriptorstest.rows; i++)
				{
					double const dist = matches[i].distance;
					if (dist < min_dist) min_dist = dist;

				}
				// Detect "good" match
				std::vector< DMatch > good_matches;
				for (auto i = 0; i < descriptorstest.rows; i++)
				{

					if (matches[i].distance <= (std::min)(matchcoeff* min_dist, matchthreshold))
					{
						cerr << matches[i].distance << endl;
						good_matches.push_back(matches[i]);
					}
				}
				// Draw only "good" matches
				Mat img_matches;
				drawMatches(imagetest.getMat(), keypointstest, imageref.getMat(), keypointsref,
					good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
					vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);



				_connexData.push(data::ImageData(img_matches));
			}
			return OK;
		}
	}
}
