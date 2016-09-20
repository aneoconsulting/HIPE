

#include <opencv2/core/mat.hpp>
#include <opencv2/video/tracking.hpp>
#include "drawRect.h"
#include "diffteo.h"
using namespace cv;
using namespace std;

int findperspective(const cv::Mat & intputArray, Mat & outputArray, std::vector<Point2f> corners);

int findperspective(cv::Mat & intputArray, Mat & outputArray, UserData userData)
{
	std::vector<Point2f> corners;
	for (int i = 0; i < userData.areaInfos.size(); i++)
	{
		corners.push_back(Point2f(userData.areaInfos[i].x, userData.areaInfos[i].y));
	}
	return findperspective(intputArray, outputArray, corners);
}

int findperspective(const cv::Mat & intputArray, Mat & outputArray, std::vector<Point2f> corners)
{
	cv::Point2f center(0, 0);

	if (intputArray.empty())
		return -1;

	cv::Mat dst = intputArray.clone();

	// Draw corner points
	cv::circle(dst, corners[0], 3, CV_RGB(255, 0, 0), 2);
	cv::circle(dst, corners[1], 3, CV_RGB(0, 255, 0), 2);
	cv::circle(dst, corners[2], 3, CV_RGB(0, 0, 255), 2);
	cv::circle(dst, corners[3], 3, CV_RGB(255, 255, 255), 2);

	// Draw mass center
	std::vector<Point2f> square_pts;
	cv::circle(dst, center, 3, CV_RGB(255, 255, 0), 2);
	Rect boundRect = boundingRect(corners);
	std::vector<Point2f> quad_pts;
	quad_pts.push_back(Point2f(corners[0].x, corners[0].y));
	quad_pts.push_back(Point2f(corners[1].x, corners[1].y));
	quad_pts.push_back(Point2f(corners[3].x, corners[3].y));
	quad_pts.push_back(Point2f(corners[2].x, corners[2].y));

	square_pts.push_back(Point2f(0, 0));
	square_pts.push_back(Point2f(boundRect.width, 0));
	square_pts.push_back(Point2f(boundRect.width, boundRect.height));
	square_pts.push_back(Point2f(0, boundRect.height));



	outputArray = Mat::zeros(boundRect.height, boundRect.width, CV_8UC1);

	cv::Mat transmtx = cv::getPerspectiveTransform(corners, square_pts);
	cv::warpPerspective(intputArray, outputArray, transmtx, outputArray.size());

	return 0;
}
