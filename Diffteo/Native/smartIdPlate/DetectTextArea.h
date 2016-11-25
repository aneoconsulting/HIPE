#pragma once
#include <opencv2/core/mat.hpp>

std::vector<cv::Mat> detectTextArea(const cv::Mat & planSrcRaw);

std::vector<cv::Rect> boundingLetters(const cv::Mat& planSrcRaw, cv::Mat & binary, float xmin = 0.1f, float xmax = 0.8f);
