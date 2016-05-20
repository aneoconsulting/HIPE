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

bool isBorderdetection(const IPDesc& desc, const Mat& mat)
{
	bool hasblack = false;
	double nbBlack = 0;
	for (int i = 0; i < desc.height; i++)
	{
		for (int j = 0; j < desc.width; j++)
		{
			if (mat.at<char>(j  + desc.corner.y, i + desc.corner.x) == 0)
			{
				nbBlack++;
			}
		}
	}
	double area = desc.height * desc.width;
	if (nbBlack / area > 0.3)
	{
		return true;
	}
	return false;
}

vector<IPDesc> drawShape(cv::Mat img, cv::Mat & mask, vector<vector<Point> > & contours)
{

	typedef vector<vector<Point>>::iterator itContour_t;
	//iterating through each contour
	vector<IPDesc> interestObject;
	Mat img_gray;
	convertGray(img, img_gray);

	for (itContour_t points = contours.begin(); points != contours.end(); ++points)
	{
		
		vector<Point> pts;

		//obtain a sequence of points of contour, pointed by the variable 'contour'
		approxPolyDP((*points), pts, arcLength(Mat((*points)), false) * 0.015, false);
		
		
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


		//	//drawing lines around the heptagon
		//	line(img, pts[0], pts[1], cvScalar(0, 255, 0), 4);
		/*	line(img, pts[1], pts[2], cvScalar(0, 255, 0), 4);
			line(img, pts[2], pts[3], cvScalar(0, 255, 0), 4);
			line(img, pts[3], pts[4], cvScalar(0, 255, 0), 4);
			line(img, pts[4], pts[5], cvScalar(0, 255, 0), 4);
			line(img, pts[5], pts[0], cvScalar(0, 255, 0), 4);
			
		}*/
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
		if (pts.size() >=6)
		{
			Point2f detectedCenter;
			float detectedRadius;
			minEnclosingCircle(Mat(pts), detectedCenter, detectedRadius);

			

			int minRadius =5;

			if (minRadius != 0 && detectedRadius < minRadius) {
				continue;
			}
			int maxRadiuss = 12;
			if (maxRadiuss != 0 && detectedRadius > maxRadiuss) {
				continue;
			}
			double realArea = contourArea(pts);
			
			if (realArea < 4)
				continue;

			Point circleCenter = detectedCenter;
			
			//ShowImage(img);
			int k = 8;

			Size sz(k * detectedRadius, k * detectedRadius);
			Mat object = Mat::zeros(sz, CV_8UC1);
			Point leftPoint;
			leftPoint.x = (circleCenter.x - (k / 2) * detectedRadius) < 0 ? 0 : (circleCenter.x - (k / 2) * detectedRadius);
			leftPoint.y = (circleCenter.y - (k / 2) * detectedRadius) < 0 ? 0 : (circleCenter.y - (k / 2) * detectedRadius);
			bool complete = false;
			int i = leftPoint.x;
			int j = leftPoint.y;

			for (i = leftPoint.x; i < ((int)(leftPoint.x + k * detectedRadius)) && i < img_gray.size().width; i++)
				for (j = leftPoint.y; j < ((int)(leftPoint.y + k * detectedRadius)) && j < img_gray.size().height; j++)
			{
				int i0 = (i - leftPoint.x) < 0 ? 0 : i - leftPoint.x;
				int j0 = (j - leftPoint.y) < 0 ? 0 : j - leftPoint.y;
				
				char value = img_gray.at<char>(j, i);
				object.at<char>(j0, i0) = value;
				char res = object.at<char>(j0, i0);
				//ShowImage(object);
				res = res;
			}
			if (j < img_gray.size().height && i < img_gray.size().width)
			{
				char res = object.at<char>(1, 0);
				//ShowImage(object);
				//ShowImage(img_gray);

				//ShowImage(object.clone())
				IPDesc desc(object.clone(), leftPoint.x, leftPoint.y, k * detectedRadius, k * detectedRadius);

				if (!isBorderdetection(desc, mask))
				{
					interestObject.push_back(desc);
					circle(img, circleCenter, detectedRadius, cvScalar(0, 0, 255), 2);
					//ShowImage(img);
				}

				
			}
			
			
			//ShowImage(object);
		}
	
		
	}

	//show the image in which identified shapes are marked   

	imshow("Result window", img);
	waitKey(0);

	return interestObject;
}
