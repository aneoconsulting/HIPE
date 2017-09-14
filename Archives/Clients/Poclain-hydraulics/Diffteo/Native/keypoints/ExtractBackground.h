#include "misc.h"

using namespace cv::xfeatures2d;

/*** Find and apply homography to warp image onto background image ***/
Mat extractBackground(cv::Mat img, cv::Mat & back);

/*** Identify background for object extraction ***/
Mat processBackgroundFilter(Mat & img, Mat & background, Mat & mask);