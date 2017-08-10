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

#include "filter/Algos/IDPlate/IDPlateTools.h"

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
				_debug = 0;
				charMinXBound = 0.1;
				charMaxXBound = 0.8;
				charMinFillRatio = 0.2;
				charMaxFillRatio = 0.8;
				charMinWidth = 8;
				charMinHeight = 20;
			}
			REGISTER_P(double, leftRatio);
			REGISTER_P(double, rightRatio);
			REGISTER_P(double, topRatio);
			REGISTER_P(int, _debug);
			REGISTER_P(double, charMinXBound);
			REGISTER_P(double, charMaxXBound);
			REGISTER_P(double, charMinFillRatio);
			REGISTER_P(double, charMaxFillRatio);
			REGISTER_P(int, charMinWidth);
			REGISTER_P(int, charMinHeight);


		public:
			HipeStatus process() override;

			bool compByDeriv(const std::pair<int, int>& a, const std::pair<int, int>& b);
			bool compRectByHPos(const cv::Rect& a, const cv::Rect& b);

		private:
			//std::vector<int> separateTextRows(const std::vector<cv::Rect> & charactersRects);
			//std::vector<std::vector<cv::Rect>> sortCharactersByRows(const cv::Mat& plateImage, const std::vector<cv::Rect>& plateCharacters, const std::vector<int>& charactersRows);
			std::vector<cv::Point> findCharactersBounds(const cv::Mat& image, const std::vector<std::vector<cv::Rect>>& charactersSorted);
			std::vector<std::vector<cv::Rect>> findLongestTextLines(int linesToFind, const std::vector<std::vector<cv::Rect>>& textList);
			std::vector<cv::Point> findCharactersLinesBounds(const std::vector<std::vector<cv::Rect>> lines, const cv::Mat& image = cv::Mat());
			std::vector<cv::Point> findPlateTextArea(const cv::Mat& plateImage, const std::vector<cv::Point> & textCorners);

			std::vector<cv::Point> findAreaTopLine(const cv::Mat& plateHorizontalLines, const std::vector<cv::Point> & textCorners);
			std::vector<cv::Point> findAreaLeftLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);
			std::vector<cv::Point> findAreaRightLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);

			cv::Point _findNextTopPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);
			cv::Point _findNextLeftPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);
			cv::Point _findNextRightPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);

			int findBestHorizontalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);
			int findBestVerticalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);
			cv::Point2f computeLinesIntersectionPoint(const cv::Vec4i& a, const cv::Vec4i& b);

			cv::Mat perspectiveCrop(const cv::Mat& plateImage, const std::vector<cv::Point>& textCorners);

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

		ADD_CLASS(IDPlateRectifier, leftRatio, rightRatio, topRatio, _debug, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, charMinWidth, charMinHeight);
	}
}