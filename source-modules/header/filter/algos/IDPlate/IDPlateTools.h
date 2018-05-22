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
#include <iostream>

#include <core/HipeStatus.h>
#include <core/HipeException.h>

#pragma warning(push, 0) 
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop) 


namespace filter
{
	namespace algos
	{
		namespace IDPlate
		{
			enum LineFilteringMethod
			{
				FILTER_INSIDE,
				FILTER_OUTSIDE
			};

			struct CompRectsByPos
			{
				bool operator()(const cv::Rect& a, const cv::Rect& b)
				{
					int ay = (a.y * 0.5);
					int ax = (a.x * 0.5);
					int by = (b.y * 0.5);
					int bx = (b.x * 0.5);

					const int delta = 10;	// 10
					if (abs(ay - by) > delta)
						return (ay < by);

					return (ax < bx);
				}
			};

			struct CompareByDeriv
			{
				bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b)
				{
					return a.first > b.first;
				}
			};


			/**
			 * \brief Data structure used to store useful informations about the properties of a line of cv::Rect
			 */
			struct LineData
			{
				/**
				 * \brief The average position on the Y axis of the Rects.
				 */
				double averageY;
				/**
				 * \brief The average width of the Rects
				 */
				double averageCharWidth;
				/**
				 * \brief The average height of the Rects
				 */
				double averageCharHeight;
				/**
				 * \brief The area of the smallest found Rect
				 */
				double minArea;
				/**
				 * \brief The area of the biggest found Rect
				 */
				double maxArea;
				/**
				 * \brief The average area of the Rects
				 */
				double averageArea;
				/**
				 * \brief The height of the shortest found Rect
				 */
				int minHeight;
				/**
				 * \brief The height of the tallest found Rect
				 */
				int maxHeight;
				/**
				 * \brief The width of the smallest found Rect
				 */
				int minWidth;
				/**
				 * \brief The width of the largest found Rect
				 */
				int maxWidth;

				LineData()
					: averageY(0), averageCharWidth(0), averageCharHeight(0), minArea(0), maxArea(0), averageArea(0), minHeight(0), maxHeight(0), minWidth(0), maxWidth(0)
				{
				}

				LineData(double averageY, double averageCharWidth, double averagCharHeight, double minArea, double maxArea, double averageArea, int minHeight, int maxHeight, int minWidth, int maxWidth)
					: averageY(averageY), averageCharWidth(averageCharWidth), averageCharHeight(averagCharHeight), minArea(minArea), maxArea(maxArea), averageArea(averageArea), minHeight(minHeight), maxHeight(maxHeight), minWidth(minWidth), maxWidth(maxWidth)
				{
				}
			};

			/**
			 * \brief Wrapper used to filter an imamge (bilateral filtering) using OpenCV
			 * \see cv::bilateralFilter()
			 * \param plateImage The image to apply the filtering on
			 * \param iterations The number of passes of filtering to apply
			 * \param diameter The diameter of the kernel to use
			 * \param sigmaColor
			 * \param sigmaSpace
			 * \param debug If set to true, the result of the filtering will be shown
			 * \param useGPU If set to true, the GPU will be prefered instead of the CPU
			 * \return The filtered image
			 */
			cv::Mat applyBilateralFiltering(const cv::Mat& plateImage, int iterations, int diameter, double sigmaColor, double sigmaSpace, bool debug = false, bool useGPU = false);


			/**
			 * \brief Wrapper used to generate a structuring element then apply a morphological transformation using OpenCV
			 * \param image The image to apply the transformation on
			 * \param morphShape The shape (cv::MorphShapes) of the transformation to apply
			 * \param morphType The type (cv::MorphTypes) of the transformation to apply
			 * \param kernelSize The size of the transformation kernel
			 * \return A new image where the transformation has been applied
			 */
			cv::Mat applyMorphTransform(const cv::Mat& image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize);

			/**
			 * \brief Search all the blobs in an image and analyse them to find the biggest one.
			 * \param binaryImage The image to analyse
			 * \param fillColor The color to fill the found blobs in. Must be < to threshold
			 * \param biggestBlobFillColor The color to fill the biggest found blob in. Must be < to threshold
			 * \param threshold The theshold used to search the blobs
			 * \param out_blobArea Output parameter. The biggest found blob's area
			 * \param debug The debug level
			 * \return The position of the biggest found blob as a cv::Point object
			 */
			cv::Point findBiggestBlobPos(cv::Mat& binaryImage, cv::Scalar fillColor, cv::Scalar biggestBlobFillColor, unsigned char threshold, float& out_blobArea, int debug = 0);

			/**
			 * \brief Find lines (Y coordinates) on an image separating multiple groups of characters. Characters must be sorted
			 * \param image The image on which to find the lines
			 * \param characters The rects representing characters extracted from the image. The rects must be sorted by coordinates
			 * \return The computed lines
			 */
			std::vector<int> splitImgByCharRows(const cv::Mat& image, const std::vector<cv::Rect>& characters);


			/**
			 * \brief Split a group of characters by lines
			 * \param characters The characters to split
			 * \param rows The coordinates of the lines separators
			 * \param image The image from where the characters and lines were extracted
			 * \param debug The debug level
			 * \return The characters sorted by coordinates and splitted in lines
			 */
			std::vector<std::vector<cv::Rect>> splitCharactersByRows(const std::vector<cv::Rect> characters, std::vector<int> rows, const cv::Mat & image, int debug = 0);
			//std::vector<cv::Rect> findPlateCharacters(const cv::Mat& plateImage, double xMinPos, double xMaxPos, bool debug = false, int contoursFillMethod = CV_FILLED, cv::Mat& binarizedImage = cv::Mat());

			/** Identify characters in the plate using their computed rects
			 * \param plateImage the raw plate ROI where the characters are located
			 * \param minPosX the minimun position on the x axis used to start searching for characters, as a ratio (0.0 - 1.0) of the image width. Every rect before this coordinate will be excluded
			 * \param minPosX the maximum position on the x axis used to stop searching for characters, as a ratio (0.0 - 1.0) of the image width. Every rect after this coordinate will be excluded
			 * \param charMinFillRatio the min percentage, as a ratio (0.0 - 1.0) of colored pixels in each character's rect used to validate a character.
			 * \param charMaxFillRatio the max percentage, as a ratio (0.0 - 1.0) of colored pixels in each character's rect used to validate a character
			 * \param charRectMinSize the minimum size (width & height) of a rect to be evaluated as a possible character (legacy value was 8,20)
			 * \param contoursFillMethod the method used to fill each found contours when looking for characters. A value < 0 will flood fill the found countoured area but a value > 0 will be used as the thickness to draw the contours
			 * \param out_binarizedImage (output) the computed binary image containing the characters contours (white characters on black background)
			 * \param debugLevel parameter used to show and draw debug information
			 */
			std::vector<cv::Rect> findPlateCharacter(const cv::Mat& plateImage, cv::Mat& out_binarizedImage, double minPosX, double maxPosX, double charMinFillRatio, double charMaxFillRatio, cv::Size charRectMinSize = cv::Size(8, 20), int contoursFillMethod = CV_FILLED, int debugLevel = 0);

			//std::vector<std::vector<cv::Rect>> extractPlateCharacters(const cv::Mat& preprocessedImage, cv::Mat& out_binarizedImage, double minPosX, double maxPosX, int minLines, int maxLines, double ratioY, double ratioHeight, double ratioWidth, const cv::Mat& dbgImage, int debug = 0);
			/**
			 * \brief
			 * \param preprocessedImage The preprocessed image of the ID plate (binarized: black characters on white background)
			 * \param out_binarizedImage (output) the computed binary image containing the characters contours (white characters on black background)
			 * \param minPosX The min position bound on the X Axis to start looking for characters
			 * \param maxPosX The max position bound on the X Axis to end searching for characters
			 * \param minLines Used to compute the biggest size a character can possibly have. The min number of lines of characters the plate ID can contain.
			 * \param maxLines Used to compute the smallest size a character can possibly have. The max number of lnes of characters the plate ID can contain. A good value is 2 * minLines - 1 (i.e. minLines + minLines - 1 interlines)
			 * \param ratioY A delta percentage of the average height of each line's characters used to valide false negative characters.
			 *  Every Character of the same line is not on the same Y Axis coordinate. ratioY is used to look around this Y coordinate.
			 *  Every character which Y coordinate is not within the bounds [Y - delta; Y + delta] will be discarded.
			 * \param ratioMinArea A delta percentage of the average area of each line's characters bounding box used to validate false negative characters.
			 *  Every character which area will be inferior than the average area - delta will be discarded.
			 * \param ratioMaxArea A delta percentage of the average area of each line's characters bounding box used to validate false negative characters.
			 *  Every character which area will be greater than the average area + delta will be discarded.
			 * \param dbgImage The color image of the ID plate used for debug purposes only.
			 * \param debug The debug level used to draw and log debug informations.
			 * \return Returns the image's found extracted characters' bounding rects sorted by coordinates and lines
			 */
			std::vector<std::vector<cv::Rect>> extractPlateCharacters(const cv::Mat& preprocessedImage, cv::Mat& out_binarizedImage, double minPosX, double maxPosX, int minLines, int maxLines, double ratioY, double ratioMinArea, double ratioMaxArea, const cv::Mat& dbgImage, int debug = 0);

			/**
			 * \brief Filter characters (rects) using their position (X axis) and their height.
			 * \param image The image where the rects were extracted
			 * \param characters The extracted rects to filter
			 * \param minPosX The minimum position (inclusive) on the X axis the rect should have to be validated
			 * \param maxPosX The maximum position (exclusive) on the X axis the rect should have to be validated
			 * \param minLines The minimum number of lines of text the image can contain (in fact the maximum number of times the taller character can be put in the image (height))
			 * \param maxLines The maximum number of lines of text the image can contain (in fact the maximum number of times the smallest character can be put in the image (height))
			 * \param out_dubiousCharacters Will output the rects who satisfy the position bounds test but not the size one. Those rects can possibly be false negatives
			 * \param dbgImage The image to use to draw debug informations
			 * \param debug The debug level to use
			 * \return All the characters who passed the position and the size tests
			 */
			std::vector<cv::Rect> filterCharactersFromSize(const cv::Mat& image, std::vector<cv::Rect> characters, double minPosX, double maxPosX, int minLines, int maxLines, std::vector<cv::Rect>& out_dubiousCharacters, const cv::Mat& dbgImage, int debug);

			/**
			 * \brief Analyze all the negative found characters to extract the false ones (their rects are similar from the valid ones on their respective lines)
			 * \param image The image on which the rects were extracted
			 * \param textLines The valid characters sorted by coordinates and by lines
			 * \param dubiousCharacters The list of all the rects there is a doubt on which will be procesed
			 * \param ratioY The ratio to apply on the rects' average height of each line to accept a dubious rect as a false negative
			 * \param ratioArea The ratio to apply on the rects' average area of each line to accept a dubious rect as a false negative
			 * \param dbgImage The image to use to draw debug informations
			 * \param debug The debug level to use
			 * \param speedUp Delete the dubious rect event if found negative to speed up the processing of the next lines
			 * \return All the found valid characters
			 */
			std::vector<cv::Rect> filterFalseNegativeChars(const cv::Mat& image, const std::vector<std::vector<cv::Rect>>& textLines, std::vector<cv::Rect>& dubiousCharacters, LineFilteringMethod filterMethod, double ratioY, double ratioMinArea, double ratioMaxArea, const cv::Mat& dbgImage, int debug = 0, bool speedUp = false);
			//std::vector<cv::Rect> filterFalseNegativeChars(const cv::Mat& image, const std::vector<std::vector<cv::Rect>>& textLines, std::vector<cv::Rect>& dubiousCharacters, LineFilteringMethod filterMethod, double ratioY, double ratioWidth, double ratioHeight, const cv::Mat & dbgImage, int debug = 0, bool speedUp = false);

			/**
			 * \brief Extract useful data from a line and store it in a LineData struct
			 * \see LineData
			 * \param line the container of the rects of the line
			 * \return The computed line data
			 */
			LineData extractLineData(const std::vector<cv::Rect>& line);

			/**
			 * \brief Convert a 3 channel image to a 1 channel one. (4 channels image are not handled)
			 * \param colorImage The image to be converted
			 * \return The converted 1 channel image
			 */
			cv::Mat convertColor2Gray(const cv::Mat& colorImage);

			/**
			 * \brief Convert a 1 channel image to a 3 channels (BGR) one (2 channels image are not handled)
			 * \param grayImage The image to be converted
			 * \return The converted 3 channels image
			 */
			cv::Mat convertGray2Color(const cv::Mat& grayImage);

			/**
			 * \brief Resizes an image (downscale) keeping its original aspect ratio
			 * \param image The image to be downscaled
			 * \param downscaleRatio The ratio used to downscale the image (i.e. ratio = 2 will divide by 2 the width and height of the image)
			 * \return The downscaled image
			 */
			cv::Mat downscaleImage(const cv::Mat& image, int downscaleRatio);

			/**
			 * \brief Shows an image in a dedicated window.
			 * \param image The image to show
			 * \param waitTime The time in ms the image will be shown. If waitTime = 0 the image will be show until a key is pressed. To not wait for a key to be pressed waitTime must be < 0
			 */
			void showImage(const cv::Mat& image, std::string name = "debug image", bool shouldDestroy = true, int waitTime = 0);

		}
	}
}
