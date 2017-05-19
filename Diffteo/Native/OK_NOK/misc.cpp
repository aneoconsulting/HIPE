#include "misc.h"
#include <iostream>
#include <fstream>

/*** Read image file and resize it toer resolution ***/
Mat getImg(char *filename, float resize_factor)
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
	Size size(width / resize_factor, height / resize_factor);

	cout << filename << " (" << height << " x " << width << " )\n";

	Mat img_ret;
	if (resize_factor != 1)
		resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	else
		return src_raw;

	cout << filename << " resized to (" << (height / resize_factor) << " x " << (width / resize_factor) << " )\n\n";

	return img_ret;
}

/*** Convert an image to grey ***/
void convertGray(Mat src, Mat & src_gray)
{
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	//blur(src_gray, src_gray, Size(3, 3));
}

/*** Get contours ***/
vector<vector<Point>> getContours(Mat src_gray)
{
	Mat canny_output;
	Mat temp = src_gray.clone();

	// Detect edges using canny, using optimal threshold value
	double thresh = threshold(src_gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);
	Canny(src_gray, canny_output, thresh, thresh*1.75, 3, true);
	ShowImage(canny_output);

	// Find contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(canny_output, contours, hierarchy, CV_FILLED, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	return contours;
}

/*** Read parameter file ***/
void read_parameterFile(string filename, vector<int> &ROIinfo, float resize_factor)
{
	string line;
	ifstream inputFile(filename);
	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			stringstream ss(line);
			int pix;

			while (ss >> pix)
			{
				ROIinfo.push_back(int(pix/resize_factor));
				if (ss.peek() == ',')
					ss.ignore();
			}
		}

		for (int i = 0; i < ROIinfo.size(); i++)
		{
			cout << ROIinfo[i] << " ";
			if (i % 4 == 3) cout << endl;
		}
		inputFile.close();
	}
	else
		std::cout << "Unable to open the parameter file." << std::endl << std::endl;
}