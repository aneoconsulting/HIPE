#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "diffteo.h"
using namespace cv;
using namespace std;

Mat quantizeImage2(Mat src, Mat& destination, int K, int maxIterations)
{
	std::vector<cv::Mat> imgRGB;
	cv::split(src, imgRGB);
	int k = 5;
	int n = src.rows * src.cols;
	cv::Mat img3xN(n, 3, CV_8U);
	for (int i = 0; i != 3; ++i)
		imgRGB[i].reshape(1, n).copyTo(img3xN.col(i));
	img3xN.convertTo(img3xN, CV_32F);
	cv::Mat bestLables;
	cv::kmeans(img3xN, k, bestLables, cv::TermCriteria(), 10, cv::KMEANS_RANDOM_CENTERS);
	bestLables = bestLables.reshape(0, src.rows);
	cv::convertScaleAbs(bestLables, bestLables, int(255 / k));
	ShowImageWait(bestLables)

	return bestLables;
}

Mat quantizeImage(Mat src, Mat& destination, int K, int maxIterations)
{
	Mat samples(src.rows * src.cols, 3, CV_32F);
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x * src.rows, z) = src.at<Vec3b>(y, x)[z];


	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, K, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	//kmeans(samples, K, labels, cv::TermCriteria(), 10, cv::KMEANS_RANDOM_CENTERS);


	Mat new_image(src.size(), src.type());
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * src.rows, 0);
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
		}

	destination = new_image;

	return destination;
}
