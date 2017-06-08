#include "misc.h"

using namespace cv::xfeatures2d;

void keypoints_SURF(Mat img)
{
	// Create SURF Detector
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);

	// Compute keypoint
	std::vector<KeyPoint> kp;
	Mat desc;
	detector->detectAndCompute(img, Mat(), kp, desc);
	std::cout << "SURF :: " << kp.size() << " Keypoints " << std::endl;

	//-- Draw keypoints
	Mat img_kp_SURF;
	drawKeypoints(img, kp, img_kp_SURF, Scalar(255,0,0), DrawMatchesFlags::DEFAULT);
	ShowImage(img_kp_SURF);
	//imshow("SURF keypoints", img_kp);
}

void keypoints_ORB(Mat img)
{
	Ptr<ORB> detector = ORB::create();
	std::vector<KeyPoint> kp;

	detector->detect(img, kp);
	std::cout << "ORB :: " << kp.size() << " Keypoints " << std::endl;
	
	//-- Draw keypoints
	Mat img_kp_ORB;
	drawKeypoints(img, kp, img_kp_ORB, Scalar(0,0,255), DrawMatchesFlags::DEFAULT);
	ShowImage(img_kp_ORB);
	//imshow("ORB keypoints", img_kp);
}

void keypoints_SIFT(Mat img)
{
	Ptr<SIFT> detector = SIFT::create();
	std::vector<KeyPoint> kp;

	detector->detect(img, kp);
	std::cout << "SIFT :: " << kp.size() << " Keypoints " << std::endl;

	//-- Draw keypoints
	Mat img_kp_SIFT;
	drawKeypoints(img, kp, img_kp_SIFT, Scalar(0,255,0), DrawMatchesFlags::DEFAULT);
	ShowImage(img_kp_SIFT);
	//imshow("ORB keypoints", img_kp);
}