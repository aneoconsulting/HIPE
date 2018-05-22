//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <vector>

#include "filter/algos/IDPlate/IDPlateTools.h"

namespace filter
{
	namespace algos
	{
		/**
		 * \var IDPlateRectifier::leftRatio
		 * Percentage of the image width used to extend and limit the search of the plate text area's left vertical line.
		 *
		 * \var IDPlateRectifier::rightRatio
		 * Percentage of the image width used to extend and limit the search of the plate text area's right vertical line.
		 *
		 * \var IDPlateRectifier::topRatio
		 * Percentage of the image height used to extend and limit the search of the plate text area's top horizontal line.
		 *
		 * \var IDPlateRectifier::_debug
		 * The desired debug level. The default level is 0 (disabled). A higher value will enable more debug informations.
		 *
		 * \var IDPlateRectifier::charMinXBound
		 * The minimum position on the X axis to search for characters in the image.
		 *
		 * \var IDPlateRectifier::charMaxXBound
		 * The maximum position on the X axis to search for characters in the image.
		 *
		 * \var IDPlateRectifier::charMinFillRatio
		 * The minimum ratio of colored pixels a character rect must contain to be accepted as a valid one.
		 *
		 * \var IDPlateRectifier::charMaxFillRatio
		 * The maximum ratio of colored pixels a character rect can contain to be accepted as a valid one.
		 *
		 * \var IDPlateRectifier::charMinWidth
		 * The minimum width a character rect must have to be accepted as a valid one.
		 *
		 * \var IDPlateRectifier::charMinHeight
		 * The minimum height a character rect must have to be accepted as a valid one.
		 */

		 /**
		  * \brief The IDPlateRectifier filter will try to extract the region of interest (where all the relative character are)
		  *        of an ID plate and rework its perspective to make it easier to read.
		  */
		class IDPlateRectifier : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(IDPlateRectifier, ()), _connexData(data::INDATA)
			{
				leftRatio = 0.05;
				rightRatio = 0.05;
				topRatio = 0.05;
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

			/**
			 * \brief Compare 2 rects by their position on the X axis only
			 * \param a The first rect
			 * \param b The second rect
			 * \return True only if first.x < second.x
			 */
			bool compRectByHPos(const cv::Rect& a, const cv::Rect& b);

		private:
			/**
			 * \brief Compute the bounds of a rectangle englobing the most relevant text lines of an ID plate image trying to match the pattern of the lines on the image
			 * \param image The ID plate's image
			 * \param charactersSorted The sorted by position characters extracted from an ID plate image
			 * \return The 4 points matching the image lines pattern and creating the ROI
			 */
			std::vector<cv::Point> findCharactersBounds(const cv::Mat& image, const std::vector<std::vector<cv::Rect>>& charactersSorted);

			/**
			 * \brief Find the linesToFind best successive lines of rectangles (cv::Rect) in an array
			 * \param linesToFind The number of successive lines to find
			 * \param textList The lines
			 * \return The extracted found lines
			 */
			std::vector<std::vector<cv::Rect>> findLongestTextLines(int linesToFind, const std::vector<std::vector<cv::Rect>>& textList);

			/**
			 * \brief Compute the raw bounds of a rectangle englobing lines of characters (cv::Rect)
			 * \param lines The lines of characters
			 * \param image For debug purpose only. The image where the characters were extracted.
			 * \return The 4 points creating the ROIs
			 */
			std::vector<cv::Point> findCharactersLinesBounds(const std::vector<std::vector<cv::Rect>> lines, const cv::Mat& image = cv::Mat());

			/**
			 * \brief Compute the bounds of a rectangle enblobing a zone of text trying to match the pattern of the lines on the image
			 * \param plateImage The image to work on
			 * \param textCorners The 4 corners delimiting the zone of text
			 * \return The 4 points matching the image lines pattern and creating the ROI
			 */
			std::vector<cv::Point> findPlateTextArea(const cv::Mat& plateImage, const std::vector<cv::Point> & textCorners);


			/**
			 * \brief Search the longest horizontal line on an image above a zone
			 * \param plateHorizontalLines An image preprocessed to only contain its horizontal lines
			 * \param textCorners The 4 corners delimiting the zone to look above of
			 * \return The 4 corners modified to take into account the found horizontal line
			 */
			std::vector<cv::Point> findAreaTopLine(const cv::Mat& plateHorizontalLines, const std::vector<cv::Point> & textCorners);

			/**
			 * \brief Search the longest vertical line on an image at the left of a zone
			 * \param plateVerticalLines An image preprocessed to only contain its vertical lines
			 * \param textCorners The 4 corners delimiting the zone to look at the left of
			 * \return The 4 corners modified to take into account the found vertical line
			 */
			std::vector<cv::Point> findAreaLeftLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);

			/**
			 * \brief Search the longest vertical line on an image at the right of a zone
			 * \param plateVerticalLines An image preprocessed to only contain its vertical lines
			 * \param textCorners The 4 corners delimiting the zone to look at the right of
			 * \return The 4 corners modified to take into account the found vertical line
			 */
			std::vector<cv::Point> findAreaRightLine(const cv::Mat& plateVerticalLines, const std::vector<cv::Point> & textCorners);

			/**
			 * \brief Scan an image upwards from a point (Y axis) searching for a pivot point (white pixel) to help the search of a horizontal white line
			 * \param plateImage The preprocessed image containing only horizontal white lines
			 * \param currPosition The position to start the search
			 * \param limit The limit position to end the search
			 * \return Te computed pivot point, or -1 if no white line found
			 */
			cv::Point _findNextTopPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);

			/**
			* \brief Scan to the left ofan image from a point (X axis) searching for a pivot point (white pixel) to help the search of a vertical white line
			* \param plateImage The preprocessed image containing only vertical white lines
			* \param currPosition The position to start the search
			* \param limit The limit position to end the search
			* \return Te computed pivot point, or -1 if no white line found
			*/
			cv::Point _findNextLeftPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);

			/**
			* \brief Scan to the right of an image from a point (X axis) searching for a pivot point (white pixel) to help the search of a vertical white line
			* \param plateImage The preprocessed image containing only vertical white lines
			* \param currPosition The position to start the search
			* \param limit The limit position to end the search
			* \return Te computed pivot point, or -1 if no white line found
			*/
			cv::Point _findNextRightPivotPoint(const cv::Mat& plateImage, const cv::Point& currPosition, const cv::Point& limit);

			/**
			 * \brief Search the longest white horizontal line starting from a pivot point and circling around it in an image.
			 * \param image The image to work on, preprocessed to only display horizontal white lines
			 * \param origin The pivot point used to search the line
			 * \param lineVec
			 * \param out_bestLineParameters
			 * \return
			 */
			int findBestHorizontalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);

			/**
			 * \brief Search the longest white vertical line starting from a pivot point and circling around it in an image.
			 * \param image The image to work on, preprocessed to only display vertical white lines
			 * \param origin The pivot point used to search the line
			 * \param lineVec
			 * \param out_bestLineParameters
			 * \return
			 */
			int findBestVerticalLine(const cv::Mat& image, const cv::Point& origin, const cv::Vec2f& lineVec, cv::Vec2f& out_bestLineParameters);

			cv::Point2f computeLinesIntersectionPoint(const cv::Vec4i& a, const cv::Vec4i& b);
			cv::Mat perspectiveCrop(const cv::Mat& plateImage, const std::vector<cv::Point>& textCorners);

		private:
			enum SearchDirection
			{
				UP,
				LEFT,
				RIGHT
			};


			/**
			 * \brief Square function
			 * \param a the number to square
			 * \return the squared value of the number
			 */
			static double square(double a)
			{
				return a * a;
			}
		};

		ADD_CLASS(IDPlateRectifier, leftRatio, rightRatio, topRatio, _debug, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, charMinWidth, charMinHeight);
	}
}
