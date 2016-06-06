#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "diffteo.h"

using namespace cv;
using namespace cv::xfeatures2d;

bool isParallelDescriptor(std::vector<Point2f> obj, std::vector<Point2f> scene)
{
	if (obj.size() == 0 || scene.size() == 0) return false;
	if (obj.size() == 1) return true;

	double thresh = 0.6;
	double refk0 = 0.;
	double refk1 = 0.;

	//Warning TODO check the object is correctly oriented into the scene before
	for (int i = 0; i < obj.size()-1; i++)
	{
		double x0 = obj[i].x,     x1 = obj[i + 1].x;		
		double y0 = obj[i].y,     y1 = obj[i + 1].y;

		double sx0 = scene[i].x,  sx1 = scene[i + 1].x;
		double sy0 = scene[i].y,  sy1 = scene[i + 1].y;

		/*
		double d0 = abs(x0 - x1),    d1 = abs(y0 - y1);
		double sd0 = abs(sx0 - sx1), sd1 = abs(sy0 - sy1);
		if ((1.0 - (d0 / sd0)) > thresh || (1.0 - (d1 / sd1)) > thresh)
			return false;
		*/

		double dx0 = sx0 - x0,     dy0 = sy0 - y0;
		double dx1 = sx1 - x1,     dy1 = sy1 - y1;
		if (abs(dx1 - dx0) > thresh || abs(dy1 - dy0) > thresh) return false;
	}

	return true;
}

bool isOnPosition(const IPDesc& desc, const std::vector<Point_<float>>& scene)
{
	for (int i = 0; i < scene.size(); i++)
	{
		if ((scene[i].x < desc.corner.x) || (scene[i].x > desc.corner.x + desc.width) ||
			(scene[i].y < desc.corner.y) || (scene[i].y > desc.corner.y + desc.height) )
			return false;
	}

	return true;
}

int CheckObjectIsPresent(Mat& queryImg, IPDesc desc, Mat & img_scene)
{
	// ShowImage(desc.descImg); ShowImage(img_scene);
	Mat & img_object = desc.descImg;
	if (!desc.descImg.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl;
		return -1;
	}


	// ------ Step 1: Detect the keypoints and extract descriptors using SURF------------
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);
	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	// ------ Step 2: Matching descriptor vectors using FLANN matcher --------------------
	FlannBasedMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	//------- Step 3a : Compute min distance between keypoints
	double max_dist = 0, min_dist = 100;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		//if (dist > max_dist) max_dist = dist;
	}
	printf("-- Min dist : %f \n", min_dist);
	//printf("-- Max dist : %f \n", max_dist);

	// ------- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	if (!isParallelDescriptor(obj, scene))
	{
		printf("Descriptors not parallel :: suspicious\n");
		rectangle(queryImg, desc.corner, Point(desc.corner.x + desc.width, desc.corner.y + desc.height), Scalar(0, 0, 255));
		//ShowImage(img_matches);
	}
	if (!isOnPosition(desc, scene))
	{
		printf("Cannot find object in correct area\n");
		rectangle(queryImg, desc.corner, Point(desc.corner.x + desc.width, desc.corner.y + desc.height), Scalar(255, 0, 0));
		//ShowImage(img_matches);
	}
	//ShowImage(queryImg);

	Mat H;
	if (!(obj.empty() || scene.empty()))
		H = findHomography(obj, scene, RANSAC, 2);
	
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); 
	obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); 
	obj_corners[3] = cvPoint(0, img_object.rows);

	std::vector<Point2f> scene_corners(4);
	if (!H.empty())
		perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), 
		Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), 
		Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), 
		Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), 
		Scalar(0, 255, 0), 4);

	//-- Show detected matches
	//imshow("Good Matches & Object detection", img_matches); waitKey(0);
	
	return 0;
}

void detectObject(Mat & queryImg, std::vector<IPDesc> interestObjects, Mat & img)
{
	for (int i = 0; i < interestObjects.size(); i++)
		CheckObjectIsPresent(queryImg, interestObjects[i], img);
	ShowImage(queryImg);

	return;
}