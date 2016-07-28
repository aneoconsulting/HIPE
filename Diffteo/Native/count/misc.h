#include "opencv2/core/core.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/videoio.hpp"
#include "opencv2/video.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // floor and ceiling functions
#include <algorithm> // for min and max functions

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

/*** Show image and wait for user input to proceed ***/
#define ShowImage(img) imshow(#img, img); waitKey(0);

/*** Exception / debug support ***/
#define THROW_EXCEPTION(ERR, MSG) \
        throw new ::Exception(ERR, MSG, __FUNCTION__, __FILE__, __LINE__)

/*** Read image file and resize it toer resolution ***/
Mat getImg(char *filename, float resize_factor, bool cvtGray);

/*** Convert an image to grey ***/
void convertGray(Mat src, Mat & src_gray);

/*** Get contours ***/
Point2i findShapesRef(Mat ref_raw, Mat ref_gray);

/*** Get contours ***/
vector<Point2i> findShapesQuery(Mat src_gray, Mat src_raw, Point2i minmaxRadius);

/*** Read ROIs from parameter file ***/
void read_parameterFile(string filename, vector<int> &ROIinfo, float resize_factor);

/*** Return a rect around an extended ROI ***/
Rect getExtendedROI(Rect initialROI, Mat img, double coef);