#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "misc.h"

#include <iostream>

using namespace std;
using namespace cv;

/*** Read image file and resize it to better resolution ***/
Mat getImg(char *filename)
{
	// Read image file
	Mat src_raw = imread(filename, 1);

	// If no filename passed or no image found, return empty matrix
	if (filename == NULL)
		return Mat();
	if (src_raw.empty())
	{
		cerr << "No image supplied ..." << filename << endl;
		return src_raw;
	}

	// Resize image to acceptable dimensions
	int width = src_raw.cols;
	int height = src_raw.rows;
	Size size(width / 5, height / 5);

	Mat img_ret;
	resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	return img_ret;
}