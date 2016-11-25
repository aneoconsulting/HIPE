#include "PerspectiveProcessing.h"


#include <opencv2/core/mat.hpp>
#include "ImageProcessing.h"
#include "diffteo.h"
#include <opencv2/video.hpp>
#include "tools.h"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "perspective.h"
#include "DetectTextArea.h"
#include <iostream>
#include "DetectGridShape.h"
#include <opencv2/cudaimgproc.hpp>

using namespace cv;
using namespace xfeatures2d;

typedef struct
{
	int contador;
	double media;
} cuadrante;

cv::Mat rotateImage(const cv::Mat& source, double angle)
{
	cv::Point2f src_center(source.cols / 2.0F, source.rows / 2.0F);
	cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, angle, 1.0);
	cv::Mat dst;
	cv::warpAffine(source, dst, rot_mat, source.size());
	return dst;
}


std::vector<Point2f> findblockPositionInImage(const Mat& tplBlock, const Mat& input)
{
	//-- Step 1: Detect the keypoints and extract descriptors using SURF
	int minHessian = 50;

	//Ptr<FastFeatureDetector> star_detector = FastFeatureDetector::create(50, false, FastFeatureDetector::TYPE_5_8);
	//Ptr<BriefDescriptorExtractor> detector = BriefDescriptorExtractor::create(32, true);
	std::vector<Point> corner_tpl;
	goodFeaturesToTrack(input, corner_tpl, 100, 0.1, 70);
	Mat debug = input.clone();
	input.convertTo(debug, CV_8UC3);

	for (int i = 0; i < corner_tpl.size(); i++)
	{
		circle(debug, corner_tpl[i], 3, Scalar(255, 0, 0), 2);
	}
	ShowImageWait(debug);

	Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create(minHessian);
	KeyPoint t;

	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	//star_detector->detect(tplBlock, keypoints_object);
	//star_detector->detect(input, keypoints_scene);
	//detector->compute(tplBlock, keypoints_object, descriptors_object);
	//detector->compute(input, keypoints_scene, descriptors_scene);

	detector->detectAndCompute(tplBlock, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(input, tplBlock, keypoints_scene, descriptors_scene);


	//-- Step 2: Matching descriptor vectors using FLANN matcher
	BFMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(descriptors_object, descriptors_scene, matches);
	double max_dist = 0;
	double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	Mat img_matches;
	//drawMatches(tplBlock, keypoints_object, input, keypoints_scene,
	//	good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	//	std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	drawMatches(tplBlock, keypoints_object, input, keypoints_scene,
	            good_matches, img_matches);
	ShowImageWait(img_matches);

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(tplBlock.cols, 0);
	obj_corners[2] = cvPoint(tplBlock.cols, tplBlock.rows);
	obj_corners[3] = cvPoint(0, tplBlock.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);
	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(tplBlock.cols, 0), scene_corners[1] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(tplBlock.cols, 0), scene_corners[2] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(tplBlock.cols, 0), scene_corners[3] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(tplBlock.cols, 0), scene_corners[0] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	ShowImageWait(img_matches);
	//line(input, scene_corners[0] + Point2f(tplBlock.cols, 0), scene_corners[1] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	//line(input, scene_corners[1] + Point2f(tplBlock.cols, 0), scene_corners[2] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	//line(input, scene_corners[2] + Point2f(tplBlock.cols, 0), scene_corners[3] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	//line(input, scene_corners[3] + Point2f(tplBlock.cols, 0), scene_corners[0] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	//ShowImageWait(input);
	/*scene_corners[0] += Point2f(tplBlock.cols, 0);
	scene_corners[1] += Point2f(tplBlock.cols, 0);
	scene_corners[2] += Point2f(tplBlock.cols, 0);
	scene_corners[3] += Point2f(tplBlock.cols, 0);
*/
	sortCorners(scene_corners);

	//Avoid to lost region
	Point2f bump_diag1;
	bump_diag1.x = abs(scene_corners[2].x - scene_corners[0].x);
	bump_diag1.y = abs(scene_corners[2].y - scene_corners[0].y);
	bump_diag1.x = bump_diag1.x * 0.05;
	bump_diag1.y = bump_diag1.y * 0.05;

	Point2f bump_diag2;
	bump_diag2.x = abs(scene_corners[1].x - scene_corners[3].x);
	bump_diag2.y = abs(scene_corners[1].y - scene_corners[3].y);
	bump_diag2.x = bump_diag2.x * 0.05;
	bump_diag2.y = bump_diag2.y * 0.05;


	scene_corners[0].x = max(0, (int)(scene_corners[0].x - bump_diag1.x));
	scene_corners[0].y = max(0, (int)(scene_corners[0].y - bump_diag1.y));
	scene_corners[1].x = min(input.cols, (int)(scene_corners[1].x + bump_diag2.x));
	scene_corners[1].y = max(0, (int)(scene_corners[1].y - bump_diag2.y));
	scene_corners[2].x = min(input.cols, (int)(scene_corners[2].x + bump_diag1.x));
	scene_corners[2].y = min(input.rows, (int)(scene_corners[2].y + bump_diag2.y));
	scene_corners[3].x = max(0, (int)(scene_corners[3].x - bump_diag1.x));;
	scene_corners[3].y = min(input.rows, (int)(scene_corners[3].y + bump_diag2.y));

	line(img_matches, scene_corners[0] + Point2f(tplBlock.cols, 0), scene_corners[1] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(tplBlock.cols, 0), scene_corners[2] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(tplBlock.cols, 0), scene_corners[3] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(tplBlock.cols, 0), scene_corners[0] + Point2f(tplBlock.cols, 0), Scalar(0, 255, 0), 4);
	ShowImageWait(img_matches);

	return scene_corners;
}

using namespace std;

bool compareByDeriv(const pair<int, int>& a, const pair<int, int>& b)
{
	return a.first > b.first;
}

bool orderByX(const Point& a, const Point& b)
{
	return a.x < b.x;
}

bool orderByY(const Point& a, const Point& b)
{
	return a.y < b.y;
}

bool orderByRectYmax(const Rect& a, const Rect& b)
{
	return a.y + a.height < b.y + b.height;
}

bool orderByRectX(const Rect& a, const Rect& b)
{
	return a.x < b.x;
}


std::vector<int> separatorLine(const std::vector<Rect_<int>>& test)
{
	vector<pair<int, int>> deriv(test.size());
	vector<pair<int, int>> deriv2(test.size());
	vector<int> result;

	deriv[0].first = 0;
	deriv[0].first = 0;
	int avg = 0;
	for (int i = 1; i < test.size(); i++)
	{
		//Ignore False EOL (i.e : do not consider letter perspective) Ignore Inclusion of letter in 90% of other
		//Then Implement if Y1 >= Y0 && Y1 <= Y0 + 0.90 * H0 then ignore it
		// OR Y0 >= Y1 && Y0 <= Y1 + 0.9 * H1
		if ((test[i - 1].y >= test[i].y && test[i - 1].y <= test[i].y + 0.9 * test[i].height) ||
			(test[i].y >= test[i - 1].y && test[i].y <= test[i - 1].y + 0.9 * test[i - 1].height))
			continue;

		deriv[i - 1].first = abs(test[i - 1].y - test[i].y);
		deriv[i - 1].second = i - 1;
		avg += deriv[i - 1].first;
	}

	avg /= test.size();
	std::sort(deriv.begin(), deriv.end(), compareByDeriv);
	/*for (int i = 1; i < deriv.size(); i++)
	{
		deriv2[i - 1].first = abs(deriv[i - 1].first - deriv[i].first);
		deriv2[i - 1].second = i - 1;
	}
	std::sort(deriv2.begin(), deriv2.end(), compareByDeriv);*/
	for (int i = 0; i < deriv.size(); i++)
	{
		if (deriv[i].first > 3)
		{
			int y = test[deriv[i].second].y + test[deriv[i].second].height;// +(deriv[deriv2[i].second].first - test[deriv[deriv2[i].second].second].height) / 2;
			/*bool isCrossed = false;
			for (int j = 0; j < test.size(); j++)
			{
				if (y >= test[j].y && y <= test[j].y + test[j].height)
					isCrossed = true;

				
			}
			if (! isCrossed)*/
				result.push_back(y);
			
		}
	}
	std::sort(result.begin(), result.end());

	return result;
}

#define SQR(a) ((a) * (a))

void stickRectangleToGrid(const Mat& input, vector<Point2f>& corners)
{
	//test with the first left line
	Point2f top_left = corners[0];
	Point2f bt_left = corners[3];
	Mat intputStickRectangleToGrid = input.clone();
	//ShowImageWait(intputStickRectangleToGrid);

	// Specify size on horizontal axis
	int horizontalsize = input.cols / 30;
	// Create structure element for extracting horizontal lines through morphology operations
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
	// Apply morphology operations
	Mat horizontal;
	erode(input, horizontal, horizontalStructure, Point(-1, -1));
	dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));

	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	dilate(horizontal, horizontal, kernel1);

	findLineAlignmentTop(horizontal, corners);


	Mat vertical;
	// Specify size on vertical axis
	int verticalsize = input.rows / 30;
	// Create structure element for extracting vertical lines through morphology operations
	Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));
	// Apply morphology operations
	erode(input, vertical, verticalStructure, Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, Point(-1, -1));


	findLineAlignmentLeft(vertical, corners);
	findLineAlignmentRight(vertical, corners);


	//double alpha = (double)(bt_left.y - top_left.y) / (double)(bt_left.x - top_left.x);
	//double b = ((double)(top_left.y)) - alpha * top_left.x;

	////get 3 points from this line
	//int xmin = min(bt_left.x, top_left.x);

	//double dist_x = abs(bt_left.x - top_left.x);
	//double stride = dist_x / 5;
	//Point2f p1, p2, p3;
	//p1.x = xmin + stride;
	//p2.x = xmin + 2 * stride;
	//p3.x = xmin + 3 * stride;

	//p1.y = alpha * p1.x + b;
	//p2.y = alpha * p2.x + b;
	//p3.y = alpha * p3.x + b;

	//vector<Point2f> plots;
	//plots.push_back(p1);
	//plots.push_back(p2);
	//plots.push_back(p3);
	//
	//

	//for (int i = 0; i < plots.size(); i++)
	//{
	//	const uchar* row = input.ptr(plots[i].y);

	//	for (int x = plots[i].x; x >= 0; x--)
	//	{
	//		if (row[x] == 255)
	//		{
	//			plots[i].x = x;
	//			break;
	//		}
	//	}
	//}
	//alpha = (double)(plots[2].y - plots[0].y) / (double)(plots[2].x - plots[0].x);
	//b = ((double)(plots[0].y)) - alpha * plots[0].x;
}

Mat binarizeImage(const Mat& input_gray)
{
	//line amplifier
	Mat edges;

	Canny(input_gray, edges, 100, 50, 3, true);
	Mat morphKernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
	morphologyEx(edges, edges, MORPH_GRADIENT, morphKernel);
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(edges.cols / 30, 1));
	// Apply morphology operations
	Mat horizontal;
	erode(edges, horizontal, horizontalStructure, Point(-1, -1));

	//ShowImageWait(edges);
	return edges;
}

vector<vector<Rect>> findBestTextLine(const vector<vector<Rect_<int>>>& textList, int nbLineRequested)
{
	vector<vector<Rect>> result(nbLineRequested);

	vector<pair<int, double>> nbDeriv(textList.size());


	for (int i = 0; i < textList.size(); i++)
	{
		if (textList[i].size() < 3) continue;

		vector<double> deriv(textList[i].size() - 1);

		vector<double> deriv2(deriv.size() - 1);

		double deriv_avg = 0.0;
		for (int j = 0; j < textList[i].size() - 1; j++)
		{
			
			deriv[j] = abs (textList[i][j + 1].y - textList[i][j].y);

			if (j == 0) continue;

			deriv_avg += abs(deriv[j] - deriv[j - 1]);
		}

		
		
		deriv_avg /= textList[i].size();
		nbDeriv[i].first = textList[i].size();
		nbDeriv[i].second = deriv_avg;
	}

	int best_index = -1;
	int best_sum_deriv2 = std::numeric_limits<int>::max();

	//Select best index by deriv second 
	for (int i = 0; i < nbDeriv.size() - nbLineRequested + 1; i++)
	{
		double sum_deriv2 = 0.0;

		if (nbDeriv[i].first == 0 || nbDeriv[i].second == 0.0)
			continue;
		if (i + nbLineRequested > nbDeriv.size())
			break;
		bool isEmpty = false;

		for (int j = 0; j < nbLineRequested; j++)
		{
			if (nbDeriv[i + j].first == 0 || nbDeriv[i + j].second == 0.0)
			{
				isEmpty = true;
				break;
			}
				

			sum_deriv2 += nbDeriv[i + j].second;
		}
		if (isEmpty) continue;

		sum_deriv2 /= (double)nbLineRequested;
		if (sum_deriv2 < best_sum_deriv2)
		{
			best_sum_deriv2 = sum_deriv2;
			best_index = i;
		}
	}

	for (int j = 0; j < nbLineRequested; j++) {
		result[j] = textList[best_index + j];
	}

	return result;
}

std::vector<Point2f>detectKeyDescriptor(const Mat& input, Mat& templ)
{
	std::vector<Point2f> res;// = findblockPositionInImage(templ, input);
	Mat input_gray;
	if (input.type() == CV_8UC3)
		cvtColor(input, input_gray, CV_RGB2GRAY);
	else
		input_gray = input;
	Mat binary;

	std::vector<Rect> test = boundingLetters(input_gray, binary);
	Mat squareOfText;
	if (input.type() == CV_8UC1)
		cvtColor(input, squareOfText, CV_GRAY2RGB);
	else
		squareOfText = input.clone();
	for (int i = 0; i < test.size(); i++)
	{
		rectangle(squareOfText, test[i], Scalar(255, 0, 0), 2);
	}
	ShowImageNoWait(squareOfText);

	std::vector<int> sep = separatorLine(test);
	sep.push_back(input.rows - 1);
	int nbLine = sep.size();
	std::vector<std::vector<Rect>> freqArea(nbLine);

	for (int i = 0; i < nbLine; i++)
	{
		line(squareOfText, Point(0, sep[i]), Point(squareOfText.cols - 1, sep[i]), Scalar(0, 0, 255));
	}
	ShowImageWait(squareOfText)
	for (int j = 0, kprev = 0, knext = sep[j]; j < nbLine; j++)
	{
		knext = sep[j];

		for (int i = 0; i < test.size(); i++)
		{
			Point center;
			center.x = test[i].x + test[i].width / 2;
			center.y = test[i].y + test[i].height / 2;

			if (center.y > kprev && center.y < knext)
			{
				freqArea[j].push_back(test[i]);
			}
		}

		kprev = knext;
	}
	Mat filterLetterImg = Mat::zeros(input.size(), CV_8UC1);
	double alpha = 0;
	int countLine = 0;
	for (int i = freqArea.size() - 1; i > freqArea.size() - 4; i--)
	{
		if (freqArea[i].size() < 2) continue;

		Rect& rect = freqArea[i][0];
		Point center;
		center.x = rect.x + rect.width / 2;
		center.y = rect.y + rect.height / 2;

		Rect& rectnext = freqArea[i][freqArea[i].size() - 1];
		Point centernext;
		centernext.x = rectnext.x + rectnext.width / 2;
		centernext.y = rectnext.y + rectnext.height / 2;

		alpha += ((double)(centernext.y - center.y)) / ((double)(centernext.x - center.x));
		countLine++;
	}
	alpha /= countLine;
	/*Point center;
	center.x = debug.cols / 2;
	center.y = debug.rows / 2;

	int b = ((double)center.y) - alpha * center.x;

	line(debug, Point(0, b), Point(debug.cols - 1, alpha * (debug.cols - 1) + b), Scalar(255, 255, 255), 2);
	ShowImageWait(debug);*/

	/*b = (freqArea[3][0].y - 4) - alpha * (freqArea[3][0].x - 4);

	res.push_back(Point(freqArea[3][0].x - 4, freqArea[3][0].y - 4));
	res.push_back(Point(freqArea[3][freqArea[3].size() - 1].x - 4, alpha * (freqArea[3][freqArea[3].size() - 1].x - 4) + b));
	
	b = (freqArea[5][0].y - 4) - alpha * (freqArea[5][0].x - 4);
	res.push_back(Point(freqArea[5][freqArea[5].size() - 1].x + 4, alpha * (freqArea[5][freqArea[5].size() - 1].x + 4) + b));

	res.push_back(Point(freqArea[5][0].x - 4, alpha * (freqArea[5][freqArea[5].size() - 1].x - 4) + b));*/

	Point top_left;
	Point top_right;
	Point bt_left;
	Point bt_right;
	int offset = 8;

	//Keep the 3 last line
	//TODO : Get the best contigous series of square rather than the 3 last of the array
	//Find best lines and keep order
	vector<vector<Rect>> best3 = findBestTextLine(freqArea, 3);

	vector<Rect> line3 = best3[2];
	vector<Rect> line2 = best3[1];
	vector<Rect> line1 = best3[0];
	sort(line1.begin(), line1.end(), orderByRectX);
	sort(line2.begin(), line2.end(), orderByRectX);
	sort(line3.begin(), line3.end(), orderByRectX);


	freqArea.clear();
	freqArea.push_back(line1);
	freqArea.push_back(line2);
	freqArea.push_back(line3);
	int xmin_left = std::numeric_limits<int>::max();
	int xmax_left = std::numeric_limits<int>::min();
	int ymin_left = std::numeric_limits<int>::max();
	int ymax_left = std::numeric_limits<int>::min();
	int xmin_right = std::numeric_limits<int>::max();
	int xmax_right = std::numeric_limits<int>::min();
	int ymin_right = std::numeric_limits<int>::max();
	int ymax_right = std::numeric_limits<int>::min();

	for (int i = 0; i < freqArea.size(); i++)
	{
		Rect& rect = freqArea[i][0];
		if (xmin_left > rect.x) xmin_left = rect.x;
		if (xmax_left < rect.x + rect.width) xmax_left = rect.x + rect.width;
		if (ymin_left > rect.y) ymin_left = rect.y;
		if (ymax_left < rect.y + rect.height)
			ymax_left = rect.y + rect.height;

		Rect& rect_right = freqArea[i][freqArea[i].size() - 1];
		if (xmin_right > rect_right.x) xmin_right = rect_right.x;
		if (xmax_right < rect_right.x + rect_right.width) xmax_right = rect_right.x + rect_right.width;
		if (ymin_right > rect_right.y) ymin_right = rect_right.y;
		if (ymax_right < rect_right.y + rect_right.height) ymax_right = rect_right.y + rect_right.height;

		/*if (ymax_left < rect_right.y + rect_right.height)		ymax_left = rect_right.y + rect_right.height;*/
	}

	top_left.x = xmin_left;
	top_left.y = ymin_left;
	top_right.x = xmax_right;
	top_right.y = ymin_right;
	double angle = -alpha * CV_PI / 2.0;

	//bt_right.x = tan(angle) * (ymax + offset - (ymin - offset)) + (xmax + offset);
	bt_right.x = xmax_right;

	//To compute the bottom y we take care of homography perspective
	//take the line line
	int yright = line3[line3.size() - 1].y + line3[line3.size() - 1].height;
	int yleft = line3[0].y + line3[0].height;

	int xright = line3[line3.size() - 1].x;
	int xleft  = line3[0].x;

	double a = (yright - yleft) / (double)(xright - xleft);
	double b = yright - a  * xright;

	bt_right.y = a *(xmax_right) + b;
	

	//bt_left.x = tan(angle) * (ymax + offset - (ymin - offset)) + (xmin - offset);
	bt_left.x = xmin_left;
	bt_left.y = a * (xmin_left) + b;; 


	res.push_back(top_left);
	res.push_back(top_right);
	res.push_back(bt_right);
	res.push_back(bt_left);
	Mat savedSquaretext = squareOfText.clone();

	line(squareOfText, res[0], res[1], Scalar(0, 0, 255), 2);
	line(squareOfText, res[1], res[2], Scalar(0, 0, 255), 2);
	line(squareOfText, res[2], res[3], Scalar(0, 0, 255), 2);
	line(squareOfText, res[3], res[0], Scalar(0, 0, 255), 2);
	ShowImageWait(squareOfText);
	squareOfText = savedSquaretext;

	binary = binarizeImage(input_gray);

	stickRectangleToGrid(binary, res);

	line(squareOfText, res[0], res[1], Scalar(0, 255, 0), 2);
	line(squareOfText, res[1], res[2], Scalar(0, 255, 0), 2);
	line(squareOfText, res[2], res[3], Scalar(0, 255, 0), 2);
	line(squareOfText, res[3], res[0], Scalar(0, 255, 0), 2);
	ShowImageWait(squareOfText);


	return res;
}

int lightCorrection(const Mat& input, Mat& output)
{
	// READ RGB color image and convert it to Lab
	cv::Mat bgr_image = input.clone();
	cv::Mat lab_image;
	cv::cvtColor(bgr_image, lab_image, CV_BGR2Lab);

	// Extract the L channel
	std::vector<cv::Mat> lab_planes(3);
	cv::split(lab_image, lab_planes); // now we have the L image in lab_planes[0]

	// apply the CLAHE algorithm to the L channel
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(4);
	cv::Mat dst;
	clahe->apply(lab_planes[0], dst);

	// Merge the the color planes back into an Lab image
	dst.copyTo(lab_planes[0]);
	cv::merge(lab_planes, lab_image);

	// convert back to RGB
	cv::Mat image_clahe;
	cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);

	output = image_clahe;

	return 0;
}


Point blobs(const Mat& outerBox)
{
	int count = 0;
	int max = -1;
	Point maxPt;

	for (int y = 0; y < outerBox.size().height; y++)
	{
		const uchar* row = outerBox.ptr(y);
		for (int x = 0; x < outerBox.size().width; x++)
		{
			if (row[x] >= 128)
			{
				int area = floodFill(outerBox, Point(x, y), CV_RGB(0, 0, 64));
				if (area > max)
				{
					maxPt = Point(x, y);
					max = area;
				}
			}
		}
	}
	return maxPt;
}

struct Data
{
	int thresh_min;
	int thresh_max;

	Mat input;
	Mat output;
};

void on_trackbar(int value, void* userdata)
{
	Data* ldata = (Data *)userdata;

	Canny(ldata->input, ldata->output, ldata->thresh_min, ldata->thresh_max, 3);
	imshow("Canny", ldata->output);
}

Rect detectMaxArea(Mat& output)
{
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	Rect crop = boundingRect(output);

	return crop;
}

Rect localizeAndbinarizeScene(const Mat& input, Mat& output)
{
	//GaussianBlur(input.clone(), output, Size(5, 5), 0);
	cuda::GpuMat cuInput; 
	cuda::GpuMat cuOutput; 

	cuInput.upload(input);
	cuOutput.upload(input);
	cuda::bilateralFilter(cuInput, cuOutput, 31, 31 * 2, 31 / 2);
	cuda::bilateralFilter(cuOutput.clone(), cuOutput, 31, 31 * 2, 31 / 2);
	cuOutput.download(output);

	
	//lightCorrection(output.clone(), output);
	//quantizeImage2(output.clone(), output, 4, 10);
	//ShowImageWait(output);

	cvtColor(output, output, CV_BGR2GRAY);
	adaptiveThreshold(output.clone(), output, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 31, -2);
	Mat binary = output.clone();
	Canny(output.clone(), output, 450, 100);
	//ShowImageWait(output);
	Mat result = Mat::zeros(output.size(), CV_8UC1);
	Mat kernel2 = (Mat_<uchar>(5, 5) << 0 , 0 , 1 , 0 , 0 ,
		0 , 0 , 1 , 0 , 0 ,
		1 , 1 , 1 , 1 , 1 ,
		0 , 0 , 1 , 0 , 0 ,
		0 , 0 , 1 , 0 , 0);
	dilate(output, output, kernel2);
	//Blobs max area
	Point maxPt = blobs(output);

	floodFill(output, maxPt, CV_RGB(255, 255, 255));
	for (int y = 0; y < output.size().height; y++)
	{
		uchar* row = output.ptr(y);
		for (int x = 0; x < output.size().width; x++)
		{
			if (row[x] == 64 && x != maxPt.x && y != maxPt.y)
			{
				int area = floodFill(output, Point(x, y), CV_RGB(0, 0, 0));
			}
		}
	}

	Rect cropArea = detectMaxArea(output);

	int width_plus = 0.05 * input.cols;
	int height_plus = 0.05 * input.rows;
	//cropArea.x = (min(0, cropArea.x - width_plus / 2));
	//cropArea.y = (min(0, cropArea.y - height_plus / 2));
	cropArea.width += width_plus;
	cropArea.height += height_plus;
	if (cropArea.x + cropArea.width > binary.cols) cropArea.width = binary.cols - cropArea.x;
	if (cropArea.y + cropArea.height > binary.rows) cropArea.height = binary.rows - cropArea.y;
	output = input(cropArea);
	ShowImageNoWait(output);
	//cvtColor(output, output, CV_BGR2GRAY);
	//adaptiveThreshold(output.clone(), output, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 31, -2

	return cropArea;
}

cv::Mat extractPlate(const Mat& inputImage)
{
	Mat resultIn;
	Mat result_tpl;
	Mat result;
	Mat intermediate;
	Mat intermediate_tpl;


	localizeAndbinarizeScene(inputImage, resultIn);
	//cvtColor(intermediate, intermediate, CV_BGR2GRAY);
	//cvtColor(intermediate_tpl, intermediate_tpl, CV_RGB2GRAY);
	//cv::threshold(intermediate, resultIn, 230, 255, cv::THRESH_BINARY | THRESH_OTSU);
	//cv::threshold(intermediate_tpl, result_tpl, 230, 255, cv::THRESH_BINARY | THRESH_OTSU);
	/*quantizeImage(intermediate, resultIn, 9, 2);
	quantizeImage(intermediate_tpl, result_tpl, 2, 2);
	ShowImageWait(resultIn);
	cvtColor(resultIn, resultIn, CV_BGR2GRAY);
	cvtColor(result_tpl, templ, CV_RGB2GRAY);*/


	//ShowImageWait(resultIn);


	std::vector<Point2f> corners = detectKeyDescriptor(resultIn, Mat());
	Mat plate;
	findperspective(resultIn, plate, corners);
	//correctRotation(resultIn, resultIn, resultIn.cols);
	ShowImageWait(plate);

	return plate;
}
