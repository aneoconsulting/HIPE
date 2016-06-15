#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/video.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "misc.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


/*** Find and apply homography to warp image onto background image ***/
Mat extractBackground(cv::Mat img, cv::Mat & back);

/*** Identify background for object extraction ***/
Mat processBackgroundFilter(Mat & img, Mat & background, Mat & mask);