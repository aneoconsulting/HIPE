#include "ExtractBackground.h"
#include "misc.h"

/*** Find and apply homography to warp image onto background image ***/
Mat extractBackground(cv::Mat img, cv::Mat & back)
{
	// Create gray copy of image
	Mat img_gray;
	convertGray(img, img_gray);

	// Create SURF detector
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);

	// Compute keypoints and their descriptors for both images
	std::vector<KeyPoint> keypoints_object, keypoints_back;
	Mat descriptors_object, descriptors_back;
	detector->detectAndCompute(img, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(back, Mat(), keypoints_back, descriptors_back);

	// Brute force matching of the descriptors
	BFMatcher bfMatcher;
	std::vector<DMatch> matches;
	bfMatcher.match(descriptors_object, descriptors_back, matches);

	if (matches.size() == 0)
	{
		std::cerr << "Cannot find any similitude in the image" << std::endl;
		return img;
	}

	// Get good matches for homography : only keep matches whose distance is under half of average distance
	double dist = 0;
	for (std::vector<DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
		dist += (*d_match).distance;
	double dist_mean = dist / matches.size();
	double threshold_dist = dist_mean * 0.5;

	std::vector<Point2f> p0, p1;
	for (std::vector<DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
	{
		if ((*d_match).distance < threshold_dist)
		{
			p0.push_back(keypoints_object[(*d_match).queryIdx].pt);
			p1.push_back(keypoints_back[(*d_match).trainIdx].pt);
		}
	}

	// Apply homography with "good" matches.
	Mat transform = findHomography(p0, p1, RANSAC);
	Mat output;
	Size s = img_gray.size();
	warpPerspective(img, output, transform, s);

	return output;
}

/*** Identify background for object extraction ***/
Mat processBackgroundFilter(Mat & img, Mat & background, Mat & mask)
{
	// Threshold for canny
	int thresh = 85;

	Mat scaledImg = extractBackground(img, background);
	img = scaledImg;

	// Create Background Subtractor objects (MOG2 approach)
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	if (pMOG2.empty())
		pMOG2 = createBackgroundSubtractorMOG2(1, 900, false);
	pMOG2->apply(background, mask);
	pMOG2->apply(img, mask);

	// Smooth the mask to reduce noise in image
	GaussianBlur(mask, mask, Size(9, 9), 3.5, 3.5);
	// Threshold mask to saturate at black and white values
	threshold(mask, mask, 10, 255, THRESH_BINARY);

	// Retrieve longest contour
	// VS : [OPT] We only need to have a look at the outtermost contours (RETR_EXTERNAL)
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(mask, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
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

	// Apply mask to detour img
	MatIterator_<char> it_mask = mask.begin<char>();
	for (auto it = img.begin<Vec3b>(); it != img.end<Vec3b>(); it++, it_mask++)
		(*it) *= ((*it_mask) != (char)0);

	ShowImage(img);
	return img;
}