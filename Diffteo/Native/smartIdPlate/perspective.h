#pragma once
#include <opencv2/core/mat.hpp>
#include "drawRect.h"

int findperspective(cv::Mat & intputArray, cv::Mat & outputArray, UserData userData);

int findperspective(const cv::Mat & intputArray, cv::Mat & outputArray, std::vector<cv::Point2f> corners);
