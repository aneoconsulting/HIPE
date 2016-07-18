#include "misc.h"

/*** Read image file and resize it toer resolution ***/
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
	float resize_factor = 1;
	Size size(width / resize_factor, height / resize_factor);

	Mat img_ret;
	resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	return img_ret;
}