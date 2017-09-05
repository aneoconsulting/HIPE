#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/video.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "diffteo.h"

using namespace cv;
using namespace std;

// Global variables
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor

/*** Read image file and resize it toer resolution ***/
Mat getImage(char *filename) 
{
	// Read image file
	Mat src_raw = imread(filename, 1);
	
	// If no filename passed or no image found, return empty matrix
	if (filename == NULL)
		return Mat();
	if (src_raw.empty())
	{
		cerr << "Image not found : " << filename << endl;
		return src_raw;
	}

	// Resize image to acceptable dimensions
	int width = src_raw.cols;
	int height = src_raw.rows;
	float resize_factor = 5; // To set
	Size size(width /resize_factor, height /resize_factor);

	Mat img_ret;
	resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	return img_ret;
}

/*** Identify background for object extraction ***/
Mat processBackgroundFilter(Mat & img, Mat & background, Mat & mask)
{
	// Threshold for canny
	int thresh = 85;

	Mat scaledImg = extractBackground(img, background);
	ShowImage(scaledImg);

	img = scaledImg;

	// Create Background Subtractor objects (MOG2 approach)
	if (pMOG2.empty())
		pMOG2 = createBackgroundSubtractorMOG2(2, 800, false); //(1, 900, false);
	pMOG2->apply(background, mask);
	pMOG2->apply(img, mask);

	// Smooth the mask to reduce noise in image
	GaussianBlur(mask, mask, Size(9,9), 3.5, 3.5);
	// Threshold mask to saturate at black and white values
	threshold(mask, mask, 10, 255, THRESH_BINARY);
	// Retrieve longest contour
	// VS : [OPT] We only need to have a look at the outtermost contours (RETR_EXTERNAL)
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(mask, contours, hierarchy, RETR_EXTERNAL /*CV_FILLED*/, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int distance = 0;
	size_t index = 0;
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

	// Draw contours
	Scalar color = Scalar(255);
	mask = Mat::zeros(mask.size(), CV_8UC1);
	drawContours(mask, contours, (int)index, color, 2, 8, hierarchy, 0, Point());
	fillConvexPoly(mask, contours[index], color);

	ShowImage(mask);
	return mask;
}

/*** Get contours ***/
vector<vector<Point>> getContours(Mat src_gray)
{
	Mat canny_output;
	Mat temp = src_gray.clone();
	
	// Detect edges using canny, using optimal threshold value
	double thresh = threshold(src_gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);
	Canny(src_gray, canny_output, thresh, thresh*1.75, 3, true);
	ShowImage(canny_output);

	// Find contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(canny_output, contours, hierarchy, CV_FILLED, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Draw contours (useless, it's exactly canny_output)
	/*
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	RNG rng(12345); // Not really useful
	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(0, 255, 0); //Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, (int)i, color, 1, 8, hierarchy, 0, Point());
	}
	*/

	return contours;
}

/*** Convert an image to gray ***/
void convertGray(Mat src, Mat & src_gray)
{
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
}

int main(int argc, char** argv)
{
	// Load images (reference, background and query)
	Mat ref_raw = getImage(argv[1]);
	Mat back = getImage(argv[2]);
	Mat query_raw = getImage(argv[3]);

	// No third argument then copy ref and pass it as a query image
	if (query_raw.empty())
	{
		cerr << "[WARNING] : No query image passed as a 3rd argument." << endl
			 << "\t Setting query to reference image (default)." << endl;
		query_raw = ref_raw;
	}

	// Set window size
	ShowImage(ref_raw);
	ShowImage(back);
	ShowImage(query_raw);// waitKey(0);
	
	Mat mask;

	processBackgroundFilter(ref_raw, back, mask);

	// Set window size
	ShowImage(ref_raw);
	ShowImage(back);
	ShowImage(query_raw);
	ShowImage(mask);

	//TODO Missing trasformation and scaling
	// VS : Mask has been computed using a smaller resized version of original image
	// We must map the mask back to original image and crop around

	Mat extractedObject = ref_raw.clone();
	ShowImage(extractedObject);
	Mat extractedQueryObject = query_raw.clone();
	ShowImage(extractedQueryObject);

	MatIterator_<char> it_mask = mask.begin<char>();

	for (MatIterator_<Vec3b> it = extractedObject.begin<Vec3b>(), itQuery = extractedQueryObject.begin<Vec3b>(); 
		it != extractedObject.end<Vec3b>(); it++, itQuery++)
	{
		if ((*it_mask) == (char)0)
		{
			(*it) = 0;
			//(*itQuery) = 0;	// TM commented line
		}
		it_mask++;
	}

	ShowImage(extractedObject);
	ShowImage(extractedQueryObject);

	// Convert images to gray and blur them
	Mat src_gray, query_gray, back_gray, extractedObject_gray, extractedQueryObject_gray;

	convertGray(ref_raw, src_gray);
	ShowImage(src_gray);
	convertGray(query_raw, query_gray);
	ShowImage(query_gray);
	convertGray(back, back_gray);
	ShowImage(back_gray);
	convertGray(extractedObject, extractedObject_gray);
	ShowImage(extractedObject_gray);
	convertGray(extractedQueryObject, extractedQueryObject_gray);
	ShowImage(extractedQueryObject_gray);

	// Get contours of image
	vector<vector<Point>> points = getContours(extractedObject_gray);
	vector<IPDesc> interestObject = drawShape(extractedObject, mask, points);
	ShowImage(extractedQueryObject);

	if (!extractedObject_gray.empty())
		detectObject(query_raw, interestObject, extractedQueryObject_gray);

	return 0;
}