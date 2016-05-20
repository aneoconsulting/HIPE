#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include "diffteo.h"
using namespace cv;
using namespace std;

/**
* @function findContours_Demo.cpp
* @brief Demo code to find contours in an image
* @author OpenCV team
*/

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

using namespace cv;
using namespace std;


// Global variables

Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
/// Function header
vector<vector<Point> > getContours(Mat src_gray);

Mat getImage(char *filename) 
{
	Mat src_raw = imread(filename, 1);
	Mat img_ret;
	if (filename == NULL)
	{
		return Mat();
	}
	if (src_raw.empty())
	{
		cerr << "No image supplied ..." << filename << endl;
		return src_raw;
	}
	int width = src_raw.cols;
	int height = src_raw.rows;
	Size size(width / 5, height / 5);
	resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	return img_ret;
}



Mat processBackgroundFilter(Mat & img, Mat & background, Mat & mask)
{
	int thresh = 85;

	Mat scaledImg = extractBackground(img, background);
	img = scaledImg;

	

	//create Background Subtractor objects
	if (pMOG2.empty()) {
		pMOG2 = createBackgroundSubtractorMOG2(1, 900, false);  //MOG2 approach
	}
	pMOG2->apply(background, mask);
	//pMOG2->apply(back2, mask);
	pMOG2->apply(img, mask);

	// smooth the mask to reduce noise in image
	GaussianBlur(mask, mask, Size(9, 9), 3.5, 3.5);

	// threshold mask to saturate at black and white values
	threshold(mask, mask, 10, 255, THRESH_BINARY);
	Mat edge;
	/// Detect edges using canny
	Canny(mask, edge, thresh, thresh * 2, 3);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int distance = 0;
	size_t index = 0;
	findContours(mask, contours, hierarchy, CV_FILLED, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (size_t i = 0; i < contours.size(); i++)
	{
		if ((contours[i]).size() > distance)
		{
			distance = (contours[i]).size();
			index = i;
		}
	}

	//erode(mask, mask, Mat(), Point(-1, -1), 3);
	//dilate(mask, mask, Mat(), Point(-1, -1), 3);
	/// Draw contours
	
	Scalar color = Scalar(255);
	mask = Mat::zeros(mask.size(), CV_8UC1);
	drawContours(mask, contours, (int)index, color, 2, 8, hierarchy, 0, Point());
	fillConvexPoly(mask, contours[index], color);

	//ShowImage(mask);

	//exit(0);


	return mask;

}

/**
* @function main
*/
int function(int, char** argv);

/**
* @function thresh_callback
*/
vector<vector<Point> > getContours(Mat src_gray)
{
	double thresh = 0;
	RNG rng(12345);
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp = src_gray.clone();
	
	thresh = threshold(src_gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);
	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 1.75, 3, true);
	//ShowImage(canny_output);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_FILLED, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, (int)i, color, 1, 8, hierarchy, 0, Point());
	}

	/// Show in a window
	//ShowImage(drawing);

	return contours;
}

void convertGray(Mat src, Mat & src_gray)
{
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
}

int function(int, char** argv)
{
	Mat ref_raw, query_raw;
	Mat src; Mat src_gray, query_gray;
	int thresh = 85;
	int max_thresh = 255;
	RNG rng(12345);

	/// Load source image
	ref_raw = getImage(argv[1]);
	query_raw = getImage(argv[3]);
	Mat back = getImage(argv[2]);
	Mat back_gray;
	Mat back2;
	Mat mask;

	//No Third argument then copy ref and pass it as a query image
	if (query_raw.empty())
	{
		cerr << "[WARNING] : no query image. Please add a third argument" << endl;
		query_raw = ref_raw;
	}

	ShowImage(query_raw);

	processBackgroundFilter(ref_raw, back, mask);


	//TODO Missing trasformation and scaling


	Mat extractedObject = ref_raw.clone();
	Mat extractedQueryObject = query_raw.clone();

	MatIterator_<char> it_mask = mask.begin<char>();

	for (MatIterator_<Vec3b> it = extractedObject.begin<Vec3b>(), itQuery = extractedQueryObject.begin<Vec3b>(); it != extractedObject.end<Vec3b>(); it++, itQuery++)
	{
		if ((*it_mask) == (char)0)
		{
			(*it) = 0;
			(*itQuery) = 0;

		}
		it_mask++;
	}

	/// Convert image to gray and blur it
	convertGray(ref_raw, src_gray);
	convertGray(query_raw, query_gray);
	convertGray(back, back_gray);


	//ShowImage(src_raw);
	ShowImage(mask);
	ShowImage(extractedObject);

	Mat extractedObject_gray;
	Mat extractedQueryObject_gray;
	cvtColor(extractedObject, extractedObject_gray, COLOR_BGR2GRAY);
	blur(extractedObject_gray, extractedObject_gray, Size(3, 3));
	convertGray(extractedQueryObject, extractedQueryObject_gray);

	vector<vector<Point>> points = getContours(extractedObject_gray);

	vector<IPDesc> interestObject = drawShape(extractedObject, mask, points);
	//ShowImage(extractedQueryObject);
	//vector<Mat> test;
	//test.push_back(extractedObject_gray);
	if (!extractedObject_gray.empty())
		detectObject(query_raw, interestObject, extractedQueryObject_gray);

	return (0);
}


int main(int argc, char **argv)
{

	function(argc, argv);
	return 0;

	/*Mat image;
	Mat img;
	img = imread(argv[1], 1);
	int width = img.cols;
	int height = img.rows;
	Size size(width / 5, height / 5);

	resize(img, image, size, 0.0, 0.0, INTER_CUBIC);

	namedWindow("Display window", CV_WINDOW_AUTOSIZE);
	imshow("Display window", image); 

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	Canny(gray, gray, 100, 200, 3);
	/// Find contours   
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	findContours(gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// Draw contours
	Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	imshow("Result window", drawing);
	waitKey(0);
	return 0;*/
}
