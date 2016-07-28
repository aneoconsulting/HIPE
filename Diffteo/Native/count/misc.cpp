#include "misc.h"
#include <iostream>
#include <fstream>

/*** Read image file and resize it. Image can be grayed before resize (option)  ***/
Mat getImg(char *filename, float resize_factor, bool cvtGray)
{
	// Read image file
	Mat src_raw = imread(filename, 1);

	// If no filename passed or no image found, return empty matrix
	if (filename == NULL)
		return Mat();
	if (src_raw.empty())
	{
		cerr << "No image supplied ..." << filename << endl;
		return src_raw;
	}

	// Convert to gray before resizing if needed
	if (cvtGray)
		convertGray(src_raw, src_raw);

	// Resize image to acceptable dimensions
	int width = src_raw.cols;
	int height = src_raw.rows;
	Size size(width / resize_factor, height / resize_factor);

	cout << filename << " (" << height << " x " << width << " )\n";

	Mat img_ret;
	if (resize_factor != 1)
		resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	else
		return src_raw;

	cout << filename << " resized to (" << (height / resize_factor) << " x " << (width / resize_factor) << " )\n\n";

	return img_ret;
}

/*** Convert an image to grey ***/
void convertGray(Mat src, Mat & src_gray)
{
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
}

/*** Get contours ***/
Point2i findShapesRef(Mat ref_raw, Mat ref_gray)
{
	Mat gray = ref_gray.clone();
	Mat cimg = ref_raw.clone();

	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, 10, 100, 30, 
				 1, min(ref_gray.cols / 2, ref_gray.rows/2));

	int minRadius = circles[0][2];
	int maxRadius = minRadius;

	ShowImage(cimg);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		if (c[2] > maxRadius) maxRadius = c[2]-1;
		if (c[2] < minRadius) minRadius = c[2]+1;
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(255, 0, 255), 3, LINE_AA); // draw circle
		circle(cimg, Point(c[0], c[1]), 2, Scalar(0, 255, 0), 3, LINE_AA); // just draw center 
	}
	ShowImage(cimg);

	return Point2i(minRadius, maxRadius);
}

/*** Get contours ***/
vector<Point2i> findShapesQuery(Mat src_raw, Mat src_gray, Point2i minmaxRadius)
{
	Mat gray = src_gray.clone();	
	Mat cimg = src_raw.clone();

	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, 10, 100, 30, 0.95*minmaxRadius.x, 1.05*minmaxRadius.y);

	vector<Point2i> centers;
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		circle(cimg, Point(c[0], c[1]), c[2], Scalar(255, 0, 255), 3, LINE_AA);
		circle(cimg, Point(c[0], c[1]), 2, Scalar(0, 255, 0), 3, LINE_AA);
		centers.push_back(Point2i(c[0], c[1]));
	}
	imshow("Query circles", cimg); waitKey(0);

	return centers;

	/*
	Mat lap_res;
	Laplacian(gray, lap_res, CV_8UC1);
	
	string window_ = "Laplacian";
	namedWindow(window_, 0);
	resizeWindow(window_, int(800 * src_gray.cols / src_gray.rows), 800);
	moveWindow(window_, 50, 50);
	imshow(window_, lap_res); waitKey(0);
	imwrite("laplacian.png", lap_res);

	//Mat kernel = getStructuringElement(MORPH_RECT, Size(1,1));
	//morphologyEx(lap_res, lap_res, MORPH_CLOSE, kernel);
	double thresh = threshold(lap_res, temp, 20, 255, THRESH_BINARY);

	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	//morphologyEx(temp, temp, MORPH_CLOSE, kernel);

	string window__ = "Threshold output";
	namedWindow(window__, 0);
	resizeWindow(window__, int(800 * src_gray.cols / src_gray.rows), 800);
	moveWindow(window__, 50, 50);
	imshow(window__, temp); waitKey(0);

	// Find contours
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));

	// Draw contours (useless, it's exactly canny_output)
	Mat drawing = Mat::zeros(src_gray.size(), CV_8UC3);
	RNG rng(12345); // Not really useful
	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, (int)i, color, 1, 8, hierarchy, 0, Point());
	}

	string window2 = "Draw contours";
	namedWindow(window2, 0);
	resizeWindow(window2, int(800 * src_gray.cols / src_gray.rows), 800);
	moveWindow(window2, 50, 50);
	imshow(window2, drawing); waitKey(0);


	exit(0);
	/*
	// Detect edges using canny, using optimal threshold value
	double thresh = threshold(src_gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);

	string window__ = "Threshold output";
	namedWindow(window__, 0);
	resizeWindow(window__, int(800 * src_gray.cols / src_gray.rows), 800);
	moveWindow(window__, 50, 50);
	imshow(window__, temp); waitKey(0);

	// Canny
	double epsilon = 0.05;
	Mat canny_output;
	Canny(src_gray, canny_output, thresh*(1-epsilon), thresh*(1+epsilon), 3, true);
	
	string window = "Canny output";
	namedWindow(window, 0);
	resizeWindow(window, int(800 * src_gray.cols / src_gray.rows), 800);
	moveWindow(window, 50, 50);
	imshow(window, canny_output); waitKey(0);
	imwrite("canny_output.png", canny_output);

	exit(0);
	*/
}

/*** Read parameter file ***/
void read_parameterFile(string filename, vector<int> &ROIinfo, float resize_factor)
{
	string line;
	ifstream inputFile(filename);
	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			stringstream ss(line);
			int pix;

			while (ss >> pix)
			{
				ROIinfo.push_back(int(pix/resize_factor));
				if (ss.peek() == ',')
					ss.ignore();
			}
		}

		for (int i = 0; i < ROIinfo.size(); i++)
		{
			cout << ROIinfo[i] << " ";
			if (i % 4 == 3) cout << endl;
		}
		inputFile.close();
	}
	else
		std::cout << "Unable to open the parameter file." << std::endl << std::endl;
}

/*** Return a rect around an extended ROI ***/
Rect getExtendedROI(Rect initialROI, Mat img, double coef)
{
	int r0 = initialROI.x, r1 = initialROI.y;
	int r2 = initialROI.width, r3 = initialROI.height;

	int topLeft_x = r0 - 0.5*r2 * (coef - 1);
	int topLeft_y = r1 - 0.5*r3 * (coef - 1);
	int ROI_width = topLeft_x + coef*r2;
	int ROI_height = topLeft_y + coef*r3;

	return Rect(max(0,topLeft_x), max(0,topLeft_y), 
		        min(img.cols-topLeft_x, topLeft_x + ROI_width), 
				min(img.rows-topLeft_y, topLeft_y + ROI_height));
}