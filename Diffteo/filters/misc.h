#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"

#include <iostream>

using namespace std;
using namespace cv;

/*** Show image and wait for user input to proceed ***/
#define ShowImage(img) imshow(#img, img); waitKey(0);

/*** Read image file and resize it toer resolution ***/
Mat getImg(char *filename);