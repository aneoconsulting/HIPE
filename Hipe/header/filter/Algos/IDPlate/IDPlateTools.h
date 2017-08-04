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

			//std::vector<cv::Rect> findPlateCharacters(const cv::Mat& plateImage, double xMinPos, double xMaxPos, bool debug = false, int contoursFillMethod = CV_FILLED, cv::Mat& binarizedImage = cv::Mat());

			/** Identify characters in the plate using their computed rects
			 * @param plateImage the raw plate ROI where the characters are located
			 * @param minPosX the minimun position on the x axis used to start searching for characters, as a ratio (0.0 - 1.0) of the image width. Every rect before this coordinate will be excluded
			 * @param minPosX the maximum position on the x axis used to stop searching for characters, as a ratio (0.0 - 1.0) of the image width. Every rect after this coordinate will be excluded
			 * @param charMinFillRatio the min percentage, as a ratio (0.0 - 1.0) of colored pixels in each character's rect used to validate a character.
			 * @param charMaxFillRatio the max percentage, as a ratio (0.0 - 1.0) of colored pixels in each character's rect used to validate a character
			 * @param charRectMinSize the minimum size (width & height) of a rect to be evaluated as a possible character (legacy value was 8,20)
			 * @param contoursFillMethod the method used to fill each found contours when looking for characters. A value < 0 will flood fill the found countoured area but a value > 0 will be used as the thickness to draw the contours
			 * @param binarizedImage output matrix of the binarized image computed while looking for characters
			 * @param debugLevel parameter used to show and draw debug information
			 */
			std::vector<cv::Rect> findPlateCharacter(const cv::Mat& plateImage, cv::Mat& out_binarizedImage, double minPosX, double maxPosX, double charMinFillRatio, double charMaxFillRatio, cv::Size charRectMinSize = cv::Size(8,20), int contoursFillMethod = CV_FILLED, int debugLevel = 0);

			cv::Mat convertColor2Gray(const cv::Mat& colorImage);
			cv::Mat convertGray2Color(const cv::Mat& grayImage);
			cv::Mat downscaleImage(const cv::Mat& image, int ratio);

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