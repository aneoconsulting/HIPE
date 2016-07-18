#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

/*** Show image and wait for user input to proceed ***/
#define ShowImage(img) imshow(#img, img); //waitKey(0);

/*** Exception / debug support ***/
#define THROW_EXCEPTION(ERR, MSG) \
        throw new ::Exception(ERR, MSG, __FUNCTION__, __FILE__, __LINE__)



/*** Read image file and resize it toer resolution ***/
Mat getImg(char *filename);

/*** Convert an image to grey ***/
void convertGray(Mat src, Mat & src_gray);

/*** Get contours ***/
vector<vector<Point>> getContours(Mat src_gray);