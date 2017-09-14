#pragma once

#include <opencv2/core/mat.hpp>

cv::Mat quantizeImage(cv::Mat inputImage, cv::Mat & destination, int K, int maxIterations = 10);

cv::Mat quantizeImage2(cv::Mat src, cv::Mat & destination, int K, int maxIterations);
