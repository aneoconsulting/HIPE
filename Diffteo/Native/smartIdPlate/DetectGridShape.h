#pragma once
#include <opencv2/core/mat.hpp>

enum MV_DIR
{
	LEFT,
	RIGHT,
	UP,
	DOWN
};


void findLineAlignmentLeft(const cv::Mat & input, std::vector<cv::Point2f> & plots);
void findLineAlignmentRight(const cv::Mat & input, std::vector<cv::Point2f> & corners);
void findLineAlignmentTop(const cv::Mat & input, std::vector<cv::Point2f> & corners);
