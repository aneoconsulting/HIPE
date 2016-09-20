#pragma once
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <fstream>
#include "tools.h"


using namespace cv;

/**
*  \brief Automatic brightness and contrast optimization with optional histogram clipping
*  \param [in]src Input image GRAY or BGR or BGRA
*  \param [out]dst Destination image
*  \param clipHistPercent cut wings of histogram at given percent tipical=>1, 0=>Disabled
*  \note In case of BGRA image, we won't touch the transparency
*/
void BrightnessAndContrastAuto(const cv::Mat &src, cv::Mat &dst, float clipHistPercent)
{

	CV_Assert(clipHistPercent >= 0);
	CV_Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));

	int histSize = 256;
	float alpha, beta;
	double minGray = 0, maxGray = 0;

	//to calculate grayscale histogram
	cv::Mat gray;
	if (src.type() == CV_8UC1) gray = src;
	else if (src.type() == CV_8UC3) cvtColor(src, gray, CV_BGR2GRAY);
	else if (src.type() == CV_8UC4) cvtColor(src, gray, CV_BGRA2GRAY);
	if (clipHistPercent == 0)
	{
		// keep full available range
		cv::minMaxLoc(gray, &minGray, &maxGray);
	}
	else
	{
		cv::Mat hist; //the grayscale histogram

		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

		// calculate cumulative distribution from the histogram
		std::vector<float> accumulator(histSize);
		accumulator[0] = hist.at<float>(0);
		for (int i = 1; i < histSize; i++)
		{
			accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
		}

		// locate points that cuts at required value
		float max = accumulator.back();
		clipHistPercent *= (max / 100.0); //make percent as absolute
		clipHistPercent /= 2.0; // left and right wings
		// locate left cut
		minGray = 0;
		while (accumulator[minGray] < clipHistPercent)
			minGray++;

		// locate right cut
		maxGray = histSize - 1;
		while (accumulator[maxGray] >= (max - clipHistPercent))
			maxGray--;
	}

	// current range
	float inputRange = maxGray - minGray;

	alpha = (histSize - 1) / inputRange;   // alpha expands current range to histsize range
	beta = -minGray * alpha;             // beta shifts current range so that minGray will go to 0

	// Apply brightness and contrast normalization
	// convertTo operates with saurate_cast
	src.convertTo(dst, -1, alpha, beta);

	// restore alpha channel from source 
	if (dst.type() == CV_8UC4)
	{
		int from_to[] = { 3, 3 };
		cv::mixChannels(&src, 4, &dst, 1, from_to, 1);
	}
	return;
}


Mat getImage(String filename, int ratio)
{
	cv::Mat src_raw = imread(filename, 1);
	Mat img_ret;

	if (src_raw.empty())
	{
		std::cerr << "No image supplied ..." << filename << std::endl;
		return src_raw;
	}
	int width = src_raw.cols;
	int height = src_raw.rows;
	Size size(width / ratio, height / ratio);
	resize(src_raw, img_ret, size, 0.0, 0.0, INTER_CUBIC);
	return img_ret;
}

inline bool fileExists(const std::string& name) {
	std::ifstream myfile(name.c_str());

	return myfile.good();
}

std::string extractExtension(const std::string & filename)
{
	size_t i = filename.rfind(".", filename.length());
	if (i != std::string::npos && (filename.length() - i) <= 4) {
		return (filename.substr(i + 1, filename.length() - i));
	}
	return "";
}

std::string extractFileName(const std::string & filename)
{
	size_t i = filename.rfind(".", filename.length());
	if (i != std::string::npos && (filename.length() - i) <= 4) {
		return (filename.substr(0, i));
	}
	return "";
}


FileInfo getFileName(const std::string& s) {

	
	FileInfo fInfo;


	size_t i = s.rfind(fInfo._separator, s.length());

	if (i == std::string::npos && s.length() != 0) {
		fInfo._filename = extractFileName(s);
		fInfo._ext = extractExtension(s);
	}

	if (i != std::string::npos) {

		fInfo._dirname = s.substr(0, i);
		fInfo._filename = extractFileName(s.substr(i + 1, s.length() - i));
		fInfo._ext = extractExtension(s.substr(i + 1, s.length() - i));
	}

	return fInfo;
}
