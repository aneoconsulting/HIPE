#pragma once
#include <iostream>

#include <core/HipeStatus.h>
#include <core/HipeException.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>

namespace filter
{
	namespace algos
	{
		namespace IDPlate
		{
			cv::Mat applyBilateralFiltering(const cv::Mat& plateImage, int iterations, int diameter, double sigmaColor, double sigmaSpace, bool debug = false, bool useGPU = false);
			cv::Mat applyMorphTransform(const cv::Mat& image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize);

			std::vector<cv::Rect> findPlateCharacters(const cv::Mat& plateImage, double xMinPos, double xMaxPos, bool debug = false, int contoursFillMethod = CV_FILLED, cv::Mat& binarizedImage = cv::Mat());

			cv::Mat convertColor2Gray(const cv::Mat& colorImage);
			cv::Mat convertGray2Color(const cv::Mat& grayImage);

			void showImage(const cv::Mat& image);

			struct CompRectsByPos
			{
				bool operator()(const cv::Rect& a, const cv::Rect& b)
				{
					int ay = (a.y * 0.5);
					int ax = (a.x * 0.5);
					int by = (b.y * 0.5);
					int bx = (b.x * 0.5);

					if (abs(ay - by) > 10)
						return (ay < by);

					return (ax < bx);
				}
			};
		}
	}
}