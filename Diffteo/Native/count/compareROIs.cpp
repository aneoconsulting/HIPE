#include "misc.h"
//#include "IPdesc.h"

#pragma once
Scalar _OK(0, 255, 64);
Scalar _NOK(0, 0, 255);
Scalar _DOUBT(255, 0, 255);

bool isParallelDescriptor(std::vector<Point2f> obj, std::vector<Point2f> scene)
{
	if (obj.size() == 0 || scene.size() == 0) return false;
	if (obj.size() == 1) return true;

	double thresh = 5;

	// TODO :: check the object is correctly oriented into the scene before
	for (int i = 0; i < obj.size() - 1; i++)
	{
		double x0 = obj[i].x, x1 = obj[i + 1].x;
		double y0 = obj[i].y, y1 = obj[i + 1].y;

		double sx0 = scene[i].x, sx1 = scene[i + 1].x;
		double sy0 = scene[i].y, sy1 = scene[i + 1].y;

		double dx0 = sx0 - x0, dy0 = sy0 - y0;
		double dx1 = sx1 - x1, dy1 = sy1 - y1;
		if (abs(dx1 - dx0) > thresh || abs(dy1 - dy0) > thresh) 
			return false;
	}

	return true;
}

bool isParallelDescriptor_avg(std::vector<Point2f> obj, std::vector<Point2f> scene, double epsilon)
{
	if (obj.size() == 0 || scene.size() == 0) return false;
	if (obj.size() == 1) return true;

	// Compute (x,y) components of average match vector
	double avg_x = 0, avg_y = 0;
	for (int i = 0; i < obj.size(); i++)
	{
		avg_x += (scene[i].x - obj[i].x);
		avg_y += (scene[i].y - obj[i].y);
	}
	avg_x /= obj.size();
	avg_y /= obj.size();

	// TODO :: check the object is correctly oriented into the scene before ?
	
	// Identify matches that are close enough to average match vector
	int v_in = 0, v_out = 0;
	for (int i = 0; i < obj.size(); i++)
	{
		double dx = (scene[i].x - obj[i].x);
		double dy = (scene[i].y - obj[i].y);

		double e_x = abs((dx - avg_x) / avg_x);
		double e_y = abs((dy - avg_y) / avg_y);
		if (e_x > epsilon || e_y > epsilon)
			v_out++;
		else
			v_in++;
	}

	if (v_in >= v_out)
		return true;
	else
		return false;

	// If (v_out >= 2* v_in) maybe we could even return NOK instead of DOUBT ?
}

/*
bool isOnPosition(const IPDesc& desc, const std::vector<Point_<float>>& scene)
{
	for (int i = 0; i < scene.size(); i++)
	{
		if ((scene[i].x < desc.corner.x) || (scene[i].x > desc.corner.x + desc.width) ||
			(scene[i].y < desc.corner.y) || (scene[i].y > desc.corner.y + desc.height))
			return false;
	}

	return true;
}
*/

bool in_ROI(int ROI[4], float coef, Point2f pt)
{
	float topLeft_x = ROI[0] - 0.5*ROI[2] * (coef - 1);
	float topLeft_y = ROI[1] - 0.5*ROI[3] * (coef - 1);

	if (pt.x < topLeft_x || pt.y < topLeft_y)
		return false;
	if (pt.x >(topLeft_x + coef*ROI[2]) || pt.y >(topLeft_y + coef*ROI[3]))
		return false;
	return true;
}


Scalar compareROIs(Mat ref_g, Mat query_g, int ROI[4], bool homography)
{
	Mat ref_gray = ref_g.clone();
	Mat query_gray = query_g.clone();
	
	// Create SURF detector
	Ptr<SURF> detector = SURF::create(400, 4, 3, false, false); // default : 100,4,3,false,false
	//Ptr<SIFT> detector = SIFT::create();

	// Compute keypoints and their descriptors for both images
	std::vector<KeyPoint> kp_ref, kp_query;
	Mat desc_ref, desc_query;
	detector->detectAndCompute(ref_gray, Mat(), kp_ref, desc_ref);
	detector->detectAndCompute(query_gray, Mat(), kp_query, desc_query);

	cout << "\nKeypoints found in (ref, query) = (" << kp_ref.size() << ", "
		<< kp_query.size() << ")\n";

	if (kp_ref.size() == 0 || kp_query.size() == 0)
	{
		cout << __FUNCTION__ << "::\t No keypoints found in one or more images [NOK]\n";
		return _NOK;
	}

	// Matching descriptor vectors using FLANN matcher 
	FlannBasedMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(desc_ref, desc_query, matches);

	if (matches.size() == 0)
	{
		cout << __FUNCTION__ << "::\t No match found between the images [NOK]\n";
		return _NOK;
	}

	// Draw all matches in the ROI (not filtered yet)
	/*
	Mat res_;
	drawMatches(ref_gray, kp_ref, query_gray, kp_query, matches, res_);

	string window_ = "Comparison (all matches)";
	namedWindow(window_, 0);
	resizeWindow(window_, res_.cols, res_.rows);
	moveWindow(window_, 50, 50);
	imshow(window_, res_); waitKey(0);
	*/

	// Compute min distance between keypoints
	double max_dist = 0, min_dist = 100;
	for (int i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
	}
	cout << "-- Min dist = " << min_dist << endl;

	// Map original ROI coordinates with regard to slice of height 2x the ROI's height
	//int ROIx[4] = { ROI[0], 0.5*ROI[3], ROI[2], ROI[3] };

	// Only keep "good" matches according to a distance criterion
	// and only keep those in the original ROI (redefined above)
	std::vector<DMatch> good_matches;
	std::vector<Point2f> obj, scene;
	
	//double match_threshold = 3 * min_dist;
	double match_threshold = min(0.3, max(2.5 * min_dist, 0.1));

	for (int i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance < match_threshold) // VS: 3*min_dist a l'origine
		{
			good_matches.push_back(matches[i]);
			obj.push_back(kp_ref[matches[i].queryIdx].pt);
			scene.push_back(kp_query[matches[i].trainIdx].pt);
		}
	}
	
	
	/*
	int* kp_ref_pos = new int[kp_ref.size()];
	float* kp_ref_mindist = new float[kp_ref.size()];
	for (int i = 0; i < kp_ref.size(); i++)
	{
		kp_ref_pos[i] = -1;
		kp_ref_mindist[i] = 1000000;
	}
	for (int i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance <= 3 * min_dist)
		{
			if (kp_ref_pos[matches[i].trainIdx] == -1 )
			{
				good_matches.push_back(matches[i]);
				obj.push_back(kp_ref[matches[i].queryIdx].pt);
				scene.push_back(kp_query[matches[i].trainIdx].pt);

				kp_ref_pos[matches[i].trainIdx] = good_matches.size()-1;
				kp_ref_mindist[matches[i].trainIdx] = matches[i].distance;
			}
			else if (matches[i].distance < kp_ref_mindist[matches[i].trainIdx])
			{
				int position = kp_ref_pos[matches[i].trainIdx];
				good_matches[position] = matches[i];
				obj[position] = kp_ref[matches[i].queryIdx].pt;
				scene[position] = kp_query[matches[i].trainIdx].pt;

				kp_ref_mindist[matches[i].trainIdx] = matches[i].distance;
			}
		}
	}
*/	

	if (good_matches.size() == 0)
	{
		cout << __FUNCTION__ << "::\t No GOOD match found between the images [NOK]\n";
		return _NOK;
	}


	if (good_matches.size() >= 5)
	{
		cout << __FUNCTION__ << "::\t Enough matches found between the images [OK]\n";
		return _OK;
	}

	// Draw "good" matches in the ROI
	Mat res;
	drawMatches(ref_gray, kp_ref, query_gray, kp_query, good_matches, res);
	
	string window = "Comparison (good matches only)";
	namedWindow(window, 0);
	resizeWindow(window, 600, 600* res.rows / res.cols);
	moveWindow(window, 50, 50);
	imshow(window, res); waitKey(0);

	if (homography)
	{
		// Apply homography with "good" matches.
		Mat transform = findHomography(obj, scene, RANSAC);
		Mat query_t;
		Size s = query_gray.size();
		warpPerspective(query_gray, query_t, transform, s);

		std::vector<KeyPoint> kp_query_t;
		Mat desc_query_t;
		detector->detectAndCompute(query_t, Mat(), kp_query_t, desc_query_t);

		FlannBasedMatcher matcher_t;
		std::vector<DMatch> matches_t;
		matcher.match(desc_ref, desc_query_t, matches_t);

		double min_dist_t = 1000000;
		for (int i = 0; i < matches_t.size(); i++)
		{
			double dist = matches_t[i].distance;
			if (dist < min_dist_t) min_dist_t = dist;
		}
		cout << "-- Min dist t = " << min_dist_t << endl;

		std::vector<DMatch> good_matches_t;
		std::vector<Point2f> obj_t, scene_t;
		for (int i = 0; i < matches_t.size(); i++)
		{
			if (matches_t[i].distance < 3 * min_dist_t)
			{
				//if (in_ROI(ROIx, 1.5, kp_query[matches[i].trainIdx].pt))
				{
					good_matches_t.push_back(matches_t[i]);
					obj_t.push_back(kp_ref[matches_t[i].queryIdx].pt);
					scene_t.push_back(kp_query_t[matches_t[i].trainIdx].pt);
				}
			}
		}
		
		// Draw "good" matches in the ROI
		Mat res_t;
		drawMatches(ref_gray, kp_ref, query_t, kp_query_t, good_matches_t, res_t);

		string window_t = "Comparison (good matches only)";
		namedWindow(window_t, 0);
		resizeWindow(window_t, 600, 600 * res.rows / res.cols);
		moveWindow(window_t, 50, 50);
		imshow(window_t, res_t); waitKey(0);

		if (!isParallelDescriptor_avg(obj_t, scene_t, 0.2))
		{
			printf("Descriptors not parallel :: suspicious\n");
			return _DOUBT;
		}
	}

	// Test for parallel descriptors
	if (!isParallelDescriptor_avg(obj, scene, 0.2))
	{
		printf("Descriptors not parallel :: suspicious\n");
		return _DOUBT;
	}

	/*
	if (!isOnPosition(desc, scene))
	{
		printf("Cannot find object in correct area\n");
		rectangle(queryImg, desc.corner, Point(desc.corner.x + desc.width, desc.corner.y + desc.height), Scalar(255, 0, 0));
		//ShowImage(img_matches);
	}
	*/

	return _OK;
}