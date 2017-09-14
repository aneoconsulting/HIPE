#pragma once
#include <vector>
#include <opencv2/core/mat.hpp>


struct UserData {
	bool clicked;
	cv::String windowName;
	cv::Mat img;
	std::vector<cv::Point> areaInfos;


};

void objectAreaDrawer(const cv::Mat & imageRef, UserData & userData);