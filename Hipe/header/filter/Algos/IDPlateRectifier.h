#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <vector>


namespace filter
{
	namespace algos
	{
		class IDPlateRectifier : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(IDPlateRectifier, ()), _connexData(data::INDATA)
			{
				leftRatio = 5.0;
				rightRatio = 5.0;
				topRatio = 5.0;
				debug_ = false;
			}
			REGISTER_P(double, leftRatio);
			REGISTER_P(double, rightRatio);
			REGISTER_P(double, topRatio);
			REGISTER_P(bool, debug_)


		public:
			HipeStatus process() override;

			bool compareRectsByPosition(const cv::Rect& a, const cv::Rect& b);
			bool compareByDeriv(const std::pair<int, int>& a, const std::pair<int, int>& b);
			bool compareRectByHorizontalPosition(const cv::Rect& a, const cv::Rect& b);
		
		private:
			std::vector<cv::Rect> findPlateCharacters(const cv::Mat& plateImage);
			std::vector<int> separateTextRows(const std::vector<cv::Rect> & charactersRects);
			std::vector<std::vector<cv::Rect>> sortCharactersByRows(const cv::Mat& plateImage, const std::vector<cv::Rect>& plateCharacters, const std::vector<int>& charactersRows);
			std::vector<cv::Point> findCharactersBounds(const cv::Mat& image, const std::vector<std::vector<cv::Rect>>& charactersSorted);
			std::vector<std::vector<cv::Rect>> findLongestTextLines(int linesToFind, const std::vector<std::vector<cv::Rect>>& textList);
			std::vector<cv::Point> findCharactersLinesBounds(const std::vector<std::vector<cv::Rect>> lines, const cv::Mat& image = cv::Mat());
			std::vector<cv::Point> findPlateTextArea(const cv::Mat& plateImage, const std::vector<cv::Point> & textCorners);
			std::vector<cv::Point> findAreaTopLine(const cv::Mat& plateHorizontalLines, const std::vector<cv::Point> & textCorners);
			cv::Point _findNextTopPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);
			int findBestHorizontalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);
			std::vector<cv::Point> findAreaLeftLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);
			cv::Point _findNextLeftPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);
			int findBestVerticalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);
			std::vector<cv::Point> findAreaRightLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);
			cv::Point _findNextRightPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);
			cv::Mat perspectiveCrop(const cv::Mat& plateImage, const std::vector<cv::Point>& textCorners);

			cv::Point2f computeLinesIntersectionPoint(const cv::Vec4i& a, const cv::Vec4i& b);

			cv::Mat applyMorphTransform(const cv::Mat& image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize);

			void showImage(const cv::Mat& image);
			cv::Mat convertColorToGrayscale(const cv::Mat& plateImageColor);
			cv::Mat convertGrayscaleToBGR(const cv::Mat& plateImageGrayscale);

			void normalizeRatios(int leftBound = 0, int rightBound = 100);
			bool isInInclusiveRange(double value, double leftBound = 0.0, double rightBound = 100.0);
		
		private:
			enum SearchDirection
			{
				UP,
				LEFT,
				RIGHT
			};

			inline double square(double a)
			{
				return a * a;
			}
		};

		ADD_CLASS(IDPlateRectifier, leftRatio, rightRatio, topRatio, debug_);
	}
}