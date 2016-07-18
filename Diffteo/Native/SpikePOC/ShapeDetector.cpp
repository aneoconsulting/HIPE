#include <opencv2/core/types.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "diffteo.h"
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;

/*** Detect if we're too close to the border by counting pure black pixels **/
bool isBorderdetection(const IPDesc& desc, const Mat& mat)
{
	double nbBlack = 0;
	for (int i = 0; i < desc.height; i++)
		for (int j = 0; j < desc.width; j++)
		{
			if (mat.at<char>(j + desc.corner.y, i + desc.corner.x) == 0)
				nbBlack++;
		}

	double area = desc.height*desc.width;
	if (nbBlack / area > 0.3)
		return true;
	else
		return false;
}

/*** Return areas of interests (opt : draw circle around each of them) ***/
vector<IPDesc> drawShape(cv::Mat img, cv::Mat & mask, vector<vector<Point>> & contours)
{
	// Declarations outside of loop
	int i,j;
	typedef vector<vector<Point>>::iterator itContour_t;
	vector<IPDesc> interestObject;
	Mat img_gray;
	convertGray(img, img_gray);

	// Radius and area thresholds (VS :: arbitrary values ?)
	int minRadius = 5;
	int maxRadius = 12;
	int areaThreshold = 4;
	int k = 8; // radius scaling factor

	// Iterate through each contour in contours
	for (itContour_t points = contours.begin(); points != contours.end(); ++points)
	{	
		// Obtain a sequence of points of contour, pointed by the variable 'contour'
		vector<Point> pts; // output
		approxPolyDP((*points), pts, arcLength(Mat((*points)), false) * 0.015, false);

		// Bunch of commented code about basic shape vertices
		/*
		//if there are 3  vertices  in the contour(It should be a triangle)
		//if (pts.size() == 3)
		//{
		//	//iterating through each point

		//	//drawing lines around the triangle
		//	
		//	line(img, pts[0], pts[1], cvScalar(255, 0, 0), 4);
		//	line(img, pts[1], pts[2], cvScalar(255, 0, 0), 4);
		//	line(img, pts[2], pts[0], cvScalar(255, 0, 0), 4);

		//}

		////if there are 4 vertices in the contour(It should be a quadrilateral)
		//else if (pts.size() == 4)
		//{
		//	

		//	//drawing lines around the quadrilateral
		//	line(img, pts[0], pts[1], cvScalar(0, 255, 0), 4);
		//	line(img, pts[1], pts[2], cvScalar(0, 255, 0), 4);
		//	line(img, pts[2], pts[3], cvScalar(0, 255, 0), 4);
		//	line(img, pts[3], pts[0], cvScalar(0, 255, 0), 4);
		//}
		//else if (pts.size() == 6)
		//{


		//	//drawing lines around the hexagon
		//	line(img, pts[0], pts[1], cvScalar(0, 255, 0), 4);
			line(img, pts[1], pts[2], cvScalar(0, 255, 0), 4);
			line(img, pts[2], pts[3], cvScalar(0, 255, 0), 4);
			line(img, pts[3], pts[4], cvScalar(0, 255, 0), 4);
			line(img, pts[4], pts[5], cvScalar(0, 255, 0), 4);
			line(img, pts[5], pts[0], cvScalar(0, 255, 0), 4);
			
		}
		////if there are 7  vertices  in the contour(It should be a heptagon)
		//else if (pts.size() == 7)
		//{
		//	

		//	//drawing lines around the heptagon
		//	line(img, pts[0], pts[1], cvScalar(0, 0, 255), 4);
		//	line(img, pts[1], pts[2], cvScalar(0, 0, 255), 4);
		//	line(img, pts[2], pts[3], cvScalar(0, 0, 255), 4);
		//	line(img, pts[3], pts[4], cvScalar(0, 0, 255), 4);
		//	line(img, pts[4], pts[5], cvScalar(0, 0, 255), 4);
		//	line(img, pts[5], pts[6], cvScalar(0, 0, 255), 4);
		//	line(img, pts[6], pts[0], cvScalar(0, 0, 255), 4);
		//}

		*/

		if (pts.size() >=6)
		{
			// Find min enclosing circle to contour polygon and compute its area
			Point2f detectedCenter;
			float detectedRadius_f;
			minEnclosingCircle(Mat(pts), detectedCenter, detectedRadius_f);

			// Check thresholds for radius and area
			if (minRadius != 0 && detectedRadius_f < minRadius)
				continue;
			if (maxRadius != 0 && detectedRadius_f > maxRadius)
				continue;
			double realArea = contourArea(pts);
			if (realArea < areaThreshold)
				continue;

			Point circleCenter = detectedCenter;
			int detectedRadius = (int) detectedRadius_f;
			int kRadius = k*detectedRadius;
			Size sz(kRadius,kRadius);
			Mat object = Mat::zeros(sz, CV_8UC1);

			Point leftPoint;
			leftPoint.x = max(0, circleCenter.x - kRadius/2);
			leftPoint.y = max(0, circleCenter.y - kRadius/2);

			for (i = leftPoint.x; i < ((int)(leftPoint.x + kRadius)) && i < img_gray.size().width; i++)
				for (j = leftPoint.y; j < ((int)(leftPoint.y + kRadius)) && j < img_gray.size().height; j++)
				{
					int i0 = i - leftPoint.x;
					int j0 = j - leftPoint.y;			
					object.at<char>(j0, i0) = img_gray.at<char>(j, i);
					//ShowImage(object);
				}

			if (j < img_gray.size().height && i < img_gray.size().width)
			{
				char res = object.at<char>(1, 0);
				IPDesc desc(object.clone(), leftPoint.x, leftPoint.y, kRadius, kRadius);

				if (!isBorderdetection(desc, mask))
				{
					interestObject.push_back(desc);
					circle(img, circleCenter, detectedRadius, cvScalar(0,0,255), 2);
				}
			}
		}
	}

	// Show the image in which identified shapes are marked   
	imshow("Result window", img);
	waitKey(0);

	return interestObject;
}