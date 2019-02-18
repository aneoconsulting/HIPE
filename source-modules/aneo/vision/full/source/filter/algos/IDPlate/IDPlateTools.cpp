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

#include <filter/algos/IDPlate/IDPlateTools.h>

#pragma warning(push, 0)
#include <opencv2/core/cuda.hpp>
#include <opencv2/videostab/global_motion.hpp>
#include <opencv2/cudaimgproc.hpp>
#pragma warning(pop)

cv::Mat filter::algos::IDPlate::applyBilateralFiltering(const cv::Mat & plateImage, int iterations, int diameter, double sigmaColor, double sigmaSpace, bool debug, bool useGPU)
{
	cv::Mat output = plateImage.clone();

	// Using CPU to compute do take time
	for (int i = 0; i < iterations; ++i)
	{
		if (!useGPU)
		{
			cv::bilateralFilter(output.clone(), output, diameter, sigmaColor, sigmaSpace);
		}
		else
		{
			if (cv::cuda::getCudaEnabledDeviceCount() > 0)
			{
				cv::cuda::GpuMat cuOutput;
				cuOutput.upload(plateImage);

				cv::cuda::bilateralFilter(cuOutput.clone(), cuOutput, diameter, sigmaColor, sigmaSpace);

				cuOutput.download(output);
			}
			else
			{
				throw HipeException("Use GPU is set to true but no enabled CUDA GPU was found.");
			}
		}

		// Debug
		if (debug)	showImage(output);
	}

	return output;
}

cv::Mat filter::algos::IDPlate::applyMorphTransform(const cv::Mat & image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize)
{
	cv::Mat output;
	cv::Mat morphKernel = cv::getStructuringElement(morphShape, kernelSize);
	cv::morphologyEx(image, output, morphType, morphKernel);

	return output;
}

cv::Point filter::algos::IDPlate::findBiggestBlobPos(cv::Mat& binaryImage, cv::Scalar fillColor, cv::Scalar biggestBlobFillColor, unsigned char threshold, float& out_blobArea, int debug)
{
	cv::Mat outputImage = binaryImage;

	int maxArea = -1;
	cv::Point maxAreaPos(-1, -1);

	// For each pixel, if pixel color is greater than threshold, it's a blob : paint it black. The biggest found area is the whole plate
	for (int y = 0; y < outputImage.size().height; y++)
	{
		const uchar* row = outputImage.ptr<uchar>(y);

		for (int x = 0; x < outputImage.size().width; x++)
		{
			if (row[x] >= threshold)
			{
				cv::Point pos(x, y);
				int currArea = cv::floodFill(outputImage, pos, fillColor);

				if (debug > 2) showImage(outputImage);

				if (currArea > maxArea)
				{
					// Fill max area in max area color
					cv::floodFill(outputImage, pos, biggestBlobFillColor);

					// Fill old max area in fill color
					if (maxAreaPos != cv::Point(-1, -1)) cv::floodFill(outputImage, maxAreaPos, fillColor);

					// Update max area value
					maxArea = currArea;

					// Update max area pos
					maxAreaPos = pos;

					// Debug
					if (debug > 1)	showImage(outputImage);
				}
			}
		}
	}

	if (debug) showImage(outputImage);

	out_blobArea = maxArea;
	return maxAreaPos;
}

std::vector<int> filter::algos::IDPlate::splitImgByCharRows(const cv::Mat & image, const std::vector<cv::Rect> & characters)
{
	std::vector<int> lines;

	if (characters.empty()) throw HipeException("[ERROR] filter::algos::IDPlateTools::splitImgByCharRows - tried to split empty container");

	std::vector<std::pair<int, int>> deriv(characters.size());
	std::vector<std::pair<int, int>> deriv2(characters.size());

	deriv[0].first = 0;
	deriv[0].second = 0;

	int mean = 0;
	const double inclusionRatio = 0.9;

	for (int i = 1; i < characters.size(); i++)
	{
		//Ignore False EOL (i.e : do not consider letter perspective) Ignore Inclusion of letter in 90% of other
		if ((characters[i - 1].y >= characters[i].y && characters[i - 1].y <= characters[i].y + inclusionRatio * characters[i].height) ||
			(characters[i].y >= characters[i - 1].y && characters[i].y <= characters[i - 1].y + inclusionRatio * characters[i - 1].height))
			continue;

		deriv[i - 1].first = abs(characters[i - 1].y - characters[i].y);
		deriv[i - 1].second = i - 1;

		mean += deriv[i - 1].first;
	}

	mean /= characters.size();

	//std::sort(deriv.begin(), deriv.end(), [this](const std::pair<int, int>& a, const std::pair<int, int>& b) { return this->compByDeriv(a, b); });
	std::sort(deriv.begin(), deriv.end(), CompareByDeriv());

	for (int i = 0; i < deriv.size(); i++)
	{
		if (deriv[i].first > 3)
		{
			int y = characters[deriv[i].second].y + characters[deriv[i].second].height;
			lines.push_back(y);

		}
	}

	std::sort(lines.begin(), lines.end());

	// Add line to bottom of image (we want rows separated by top and bottom lines)
	lines.push_back(image.rows - 1);

	return lines;
}


std::vector<std::vector<cv::Rect>> filter::algos::IDPlate::splitCharactersByRows(const std::vector<cv::Rect> characters, std::vector<int> rows, const cv::Mat & image, int debug)
{
	// Output
	const int linesCount = rows.size();
	std::vector<std::vector<cv::Rect>> sortedCharacters(linesCount);

	cv::Mat dbgImage;
	// Debug
	if (debug)
	{
		dbgImage = image.clone();
		for (int i = 0; i < linesCount; i++)
		{
			cv::line(dbgImage, cv::Point(0, rows[i]), cv::Point(dbgImage.cols - 1, rows[i]), cv::Scalar(0, 0, 255));
		}

		showImage(dbgImage);
	}

	for (auto & character : characters)
	{
		// Compute center
		cv::Point charRectCenter;
		charRectCenter.x = character.x + character.width * 0.5;
		charRectCenter.y = character.y + character.height * 0.5;

		// Debug
		if (debug > 2)
		{
			cv::circle(dbgImage, charRectCenter, 4, cv::Scalar(255, 255, 0), 4);
			showImage(dbgImage);
		}

		// Find prev & next line
		for (int idx = 0, prevRow = 0, nextRow = 0; idx < linesCount; ++idx)
		{
			nextRow = rows[idx];
			if (charRectCenter.y > prevRow && charRectCenter.y < nextRow)
			{
				sortedCharacters[idx].push_back(character);
				break;
			}
			prevRow = nextRow;
		}
	}

	// Debug
	if (debug)
	{
		for (auto & row : sortedCharacters)
		{
			dbgImage = image.clone();
			for (auto & character : row)
			{
				cv::rectangle(dbgImage, character, cv::Scalar(255, 0, 0), 2);
			}
			showImage(dbgImage);
		}
	}
	return sortedCharacters;
}

std::vector<cv::Rect> filter::algos::IDPlate::findPlateCharacter(const cv::Mat& plateImage, cv::Mat& out_binarizedImage, double minPosX, double maxPosX, double charMinFillRatio, double charMaxFillRatio, cv::Size charRectMinSize, int contoursFillMethod, int debugLevel)
{
	cv::Mat imageCopy = plateImage.clone();
	cv::Mat imageGrayscale = filter::algos::IDPlate::convertColor2Gray(imageCopy);

	cv::Mat debugImage = filter::algos::IDPlate::convertGray2Color(imageGrayscale);

	//// Preprocess image to improve recognition
	// First morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));
	// Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Second morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));

	// Output "binarized" plate image
	out_binarizedImage = imageGrayscale.clone();

	// Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat contoursDebug = imageGrayscale.clone();
	cv::findContours(contoursDebug, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	// Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(contoursDebug);

	//// Find characters
	std::vector<cv::Rect> charactersRects;
	cv::Mat mask = cv::Mat::zeros(imageGrayscale.size(), CV_8UC1);

	//const cv::Size minRectSize(8, 20);
	const double maxRectWidth = 0.1 * imageGrayscale.cols;

	minPosX *= imageGrayscale.cols;
	maxPosX *= imageGrayscale.cols;

	// Debug
	// Search area
	cv::line(debugImage, cv::Point(minPosX, 0), cv::Point(minPosX, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);
	cv::line(debugImage, cv::Point(maxPosX, 0), cv::Point(maxPosX, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);
	if (debugLevel)	filter::algos::IDPlate::showImage(debugImage);

	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect characterRect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, characterRect);
		maskROI = cv::Scalar(0, 0, 0);

		// fill the contours (if countoursFillMethod/Thichness is < 0 the shape will be filled, either way it will use the value as a line thickness parametter
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), contoursFillMethod);

		// Debug
		if (debugLevel > 2)	filter::algos::IDPlate::showImage(mask);

		// Ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		// Debug
		if (debugLevel > 2)
		{
			cv::rectangle(debugImage, characterRect, cv::Scalar(255, 0, 0), 2);
			filter::algos::IDPlate::showImage(debugImage);
		}

		// Assume that at least 45% of the rect area is filled if it contains text
		//  Extracted if statment for debugging
		bool ratioBool = ratio >= charMinFillRatio && ratio < charMaxFillRatio;
		// Constraints on rect size
		bool sizeBool = characterRect.height >= charRectMinSize.height && characterRect.width >= charRectMinSize.width;
		// These two conditions alone are not very robust. Better use something like
		// the number of significant peaks in a horizontal projection as a third condition
		bool maxWidthBool = characterRect.width <= maxRectWidth;
		// Bug when the image is already cropped
		bool minPosBool = characterRect.x >= minPosX;
		bool maxPosBool = characterRect.x <= maxPosX;

		if (ratioBool && sizeBool && maxWidthBool && minPosBool && maxPosBool)
		{
			charactersRects.push_back(characterRect);

			// Debug
			if (debugLevel > 2)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 255, 0), 2);
		}

		// Debug
		else
		{

			if (debugLevel > 2)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 0, 255), 2);
		}

		//Debug
		if (debugLevel > 2)	filter::algos::IDPlate::showImage(debugImage);
	}

	//Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(debugImage);

	// Sort found characters by position
	//std::sort(charactersRects.begin(), charactersRects.end(), [](const cv::Rect& a, const cv::Rect& b) { filter::algos::IDPlate::compRectsByPos(a, b); });
	std::sort(charactersRects.begin(), charactersRects.end(), CompRectsByPos());

	return charactersRects;
}

std::vector<std::vector<cv::Rect>> filter::algos::IDPlate::extractPlateCharacters(const cv::Mat & preprocessedImage, cv::Mat & out_binarizedImage, double minPosX, double maxPosX, int minLines, int maxLines, double ratioY, double ratioMinArea, double ratioMaxArea, const cv::Mat& dbgImage, int debug)
{
	// 1. Preprocess Image
	cv::Mat imageGrayscale = convertColor2Gray(preprocessedImage);

	// First morphological transform (gradient is the difference between erosion and dilation. The result is the outline of the object)
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));

	// Debug (show first morphology preprocess)
	if (debug)	showImage(imageGrayscale, "gradient");

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Debug (show converted (binary) image)
	if (debug)	showImage(imageGrayscale, "threshold");

	// Second morphological transform (close is dilation then erosion. Closes small holes in the object)
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));

	// Debug (show first close operation)
	if (debug)	showImage(imageGrayscale, "close");

	//Output "binarized" plate image
	out_binarizedImage = imageGrayscale.clone();

	// Debug (show second morphology transform)
	if (debug)	showImage(imageGrayscale);

	// 2. Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat contoursDebug = imageGrayscale.clone();
	cv::findContours(contoursDebug, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	// Debug (show found contours)
	if (debug)	showImage(contoursDebug);

	// Debug
	cv::Mat temp;
	if (debug)
	{
		cv::Mat temp2 = imageGrayscale.clone();
		cv::cvtColor(temp2, temp, cv::COLOR_GRAY2BGR);
	}

	// 3. Extract rects from found contours
	std::vector<cv::Rect> charactersFromContours;
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect currRect = cv::boundingRect(contours[idx]);

		charactersFromContours.push_back(currRect);

		// Debug (show current found contours' englobing rect)
		if (debug)
		{
			cv::rectangle(temp, charactersFromContours.back(), cv::Scalar(255, 0, 0), 2);
			if (debug > 1)
			{
				showImage(temp);
			}
		}
	}

	// Debug (show all found contours's englobing rects)
	if (debug)	showImage(temp);

	// 4. First filtering (position and height)
	std::vector<cv::Rect> validCharacters, dubiousCharacters;
	validCharacters = filterCharactersFromSize(imageGrayscale, charactersFromContours, minPosX, maxPosX, minLines, maxLines, dubiousCharacters, dbgImage, debug);

	// 5. Sort characters
	std::sort(validCharacters.begin(), validCharacters.end(), CompRectsByPos());
	std::sort(dubiousCharacters.begin(), dubiousCharacters.end(), CompRectsByPos());

	// Find lines
	std::vector<int> linesPositions = splitImgByCharRows(imageGrayscale, validCharacters);

	// Debug (show computed separator lines)
	if (debug)
	{
		temp = dbgImage.clone();

		for (auto & row : linesPositions)
		{
			cv::line(temp, cv::Point(0, row), cv::Point(temp.cols - 1, row), cv::Scalar(0, 0, 255), 4);
		}

		showImage(temp);
	}

	// 6. Split characters in lines
	std::vector<std::vector<cv::Rect>> textLines = splitCharactersByRows(validCharacters, linesPositions, dbgImage, debug);

	// 7. Second filtering (false negative inside first and last rects of line)
	std::vector<cv::Rect> falseNegativeChars = filterFalseNegativeChars(imageGrayscale, textLines, dubiousCharacters, LineFilteringMethod::FILTER_INSIDE, ratioY, ratioMinArea, ratioMaxArea, dbgImage, debug, false);


	// 8. Add newly found characters to valid ones
	validCharacters.insert(validCharacters.end(), falseNegativeChars.begin(), falseNegativeChars.end());

	// 9. Sort and split by lines again
	std::sort(validCharacters.begin(), validCharacters.end(), CompRectsByPos());
	std::sort(dubiousCharacters.begin(), dubiousCharacters.end(), CompRectsByPos());
	textLines = splitCharactersByRows(validCharacters, linesPositions, dbgImage, debug);

	// 10. Third filtering (false negative outside first and last rects of line)
	falseNegativeChars = filterFalseNegativeChars(imageGrayscale, textLines, dubiousCharacters, LineFilteringMethod::FILTER_OUTSIDE, ratioY, ratioMinArea, ratioMaxArea, dbgImage, debug, false);

	// 8. Add newly found characters to valid ones
	validCharacters.insert(validCharacters.end(), falseNegativeChars.begin(), falseNegativeChars.end());

	// 9. Sort and split by lines again
	std::sort(validCharacters.begin(), validCharacters.end(), CompRectsByPos());
	textLines = splitCharactersByRows(validCharacters, linesPositions, dbgImage, debug);

	return textLines;
}

std::vector<cv::Rect> filter::algos::IDPlate::filterCharactersFromSize(const cv::Mat& image, std::vector<cv::Rect> characters, double minPosX, double maxPosX, int minLines, int maxLines, std::vector<cv::Rect>& out_dubiousCharacters, const cv::Mat& dbgImage, int debug)
{
	//// Filter characters (1st pass)
	// From position (X axis)
	const int minXBound = minPosX * image.cols;
	const int maxXBound = maxPosX * image.cols;

	const int maxHeight = image.rows / minLines;
	const int minHeight = image.rows / maxLines;

	// Debug (show position bounds)
	cv::Mat temp;
	if (debug > 1)
	{
		temp = dbgImage.clone();
		cv::line(temp, cv::Point(minXBound, 0), cv::Point(minXBound, temp.rows - 1), cv::Scalar(255, 0, 255), 2);
		cv::line(temp, cv::Point(maxXBound, 0), cv::Point(maxXBound, temp.rows - 1), cv::Scalar(255, 0, 255), 2);

		showImage(temp);
	}

	std::vector<cv::Rect> filteredCharacters, dubiousCharacters;
	for (auto & character : characters)
	{
		// Debug (show processed rect in blue)
		if (debug > 2)
		{
			cv::rectangle(temp, character, cv::Scalar(255, 0, 0), 1);
			cv::circle(temp, cv::Point(character.x, character.y), 2, cv::Scalar(0, 0, 255), 2);

			showImage(temp);
		}

		// Filter rect
		// TODO: Should filtering be inclusive or exclusive ?
		const bool position = character.x >= minXBound && character.x < maxXBound ? true : false;
		const bool size = character.height >= minHeight && character.height < maxHeight ? true : false;

		// Assert rect is within position (image X axis) bounds
		if (position)
		{
			// If size is correct accept char
			if (size)
			{
				filteredCharacters.push_back(character);

				// Debug (paint validated rect in green)
				if (debug > 2)
				{
					cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 1);
				}
			}
			// If not, it in fact can possibly be a good one. Put it aside for a second pass validation test
			// but, either way, we do not want the rects outside the position bounds to be checked again.
			else
			{
				dubiousCharacters.push_back(character);

				// Debug (paint doubt on rect in yellow)
				if (debug > 2)
				{
					cv::rectangle(temp, character, cv::Scalar(255, 255, 0), 1);
				}
			}
		}
		else
		{
			// Debug (paint rejected rect in red)
			if (debug > 2)
			{
				cv::rectangle(temp, character, cv::Scalar(0, 0, 255), 1);
			}
		}

		// Debug (show result of rect analysis)
		if (debug > 2) showImage(temp);
	}

	// Debug (show all validated and dubious rects)
	if (debug)
	{
		temp = dbgImage.clone();
		for (auto & character : filteredCharacters)
		{
			cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 1);
		}
		for (auto & character : dubiousCharacters)
		{
			cv::rectangle(temp, character, cv::Scalar(255, 255, 0), 1);
		}

		showImage(temp);
	}

	out_dubiousCharacters = dubiousCharacters;
	return filteredCharacters;
}

std::vector<cv::Rect> filter::algos::IDPlate::filterFalseNegativeChars(const cv::Mat& image, const std::vector<std::vector<cv::Rect>> & textLines, std::vector<cv::Rect>& dubiousCharacters, LineFilteringMethod filterMethod, double ratioY, double ratioMinArea, double ratioMaxArea, const cv::Mat & dbgImage, int debug, bool speedUp)
{
	// Extract useful data from text lines
	std::vector<LineData> charactersLinesData;
	for (auto & line : textLines)
	{
		LineData lineData = extractLineData(line);
		charactersLinesData.push_back(lineData);
	}

	// Debug (show computed average Y values)
	cv::Mat temp;
	if (debug)
	{
		temp = dbgImage.clone();
		for (auto & lineData : charactersLinesData)
		{
			cv::line(temp, cv::Point(0, lineData.averageY), cv::Point(temp.cols - 1, lineData.averageY), cv::Scalar(0, 0, 255), 2);
		}

		showImage(temp);
	}

	// Refilter characters using median value (the ones between 1st and last character on line)
	std::vector<cv::Rect> validCharacters;
	for (int lineIdx = 0; lineIdx < textLines.size(); ++lineIdx)
	{
		// Assert characters will be between 1st and last rect
		const int minXPos = textLines[lineIdx].front().x;
		const int maxXPos = textLines[lineIdx].back().x + textLines[lineIdx].back().width;

		const LineData& lineData = charactersLinesData[lineIdx];

		// Delta in pixels used to validateCharacters
		const double deltaY = ratioY * lineData.averageCharHeight;
		const double minArea = ratioMinArea * lineData.averageArea;
		const double maxArea = ratioMaxArea * lineData.averageArea;

		// Analyze every dubious character
		for (std::vector<cv::Rect>::iterator itDubiousChar = dubiousCharacters.begin(); itDubiousChar != dubiousCharacters.end();)
		{
			// Debug (show min X and max X computed from first and last character of line and show current dubious rect)
			if (debug > 2)
			{
				temp = dbgImage.clone();
				// X Bounds
				cv::line(temp, cv::Point(minXPos, 0), cv::Point(minXPos, temp.rows - 1), cv::Scalar(255, 0, 255), 2);
				cv::line(temp, cv::Point(maxXPos, 0), cv::Point(maxXPos, temp.rows - 1), cv::Scalar(255, 0, 255), 2);

				// Average Y
				cv::line(temp, cv::Point(0, lineData.averageY), cv::Point(temp.cols - 1, lineData.averageY), cv::Scalar(127, 0, 127), 2);
				cv::line(temp, cv::Point(0, lineData.averageY - deltaY), cv::Point(temp.cols - 1, lineData.averageY - deltaY), cv::Scalar(255, 0, 255), 1);
				cv::line(temp, cv::Point(0, lineData.averageY + deltaY), cv::Point(temp.cols - 1, lineData.averageY + deltaY), cv::Scalar(255, 0, 255), 1);

				// Dubious rect
				cv::rectangle(temp, *itDubiousChar, cv::Scalar(255, 255, 0), 2);
				// Dubious rect Y
				cv::circle(temp, cv::Point(itDubiousChar->x, itDubiousChar->y + itDubiousChar->height / 2.0), 2, cv::Scalar(255, 255, 0), 2);

				showImage(temp);
			}

			// Apply select chosen filter method
			bool filteringCondition = false;;
			switch (filterMethod)
			{
			case LineFilteringMethod::FILTER_INSIDE:
				// Character must be INSIDE first and last known rects of line 
				if (itDubiousChar->x >= minXPos && itDubiousChar->x <= maxXPos)	filteringCondition = true;
				break;
			case LineFilteringMethod::FILTER_OUTSIDE:
				// Character must be OUTSIDE first and last known rects of line
				if (!(itDubiousChar->x >= minXPos && itDubiousChar->x <= maxXPos))	filteringCondition = true;
				break;
			default:
				throw HipeException("[ERROR] IDPlateTools::filterFalseNegativeChars - Unknown filter method");
			}

			// Character must be INSIDE first and last known rects of line 
			if (filteringCondition)
			{
				// Character must be near median Y line and similar to others
				const double middleY = itDubiousChar->y + itDubiousChar->height / 2.0;
				const double area = itDubiousChar->width * itDubiousChar->height;

				const bool bMiddle = middleY >= lineData.averageY - deltaY && middleY <= lineData.averageY + deltaY;
				//const bool bArea = area >= lineData.averageArea - deltaArea && area <= lineData.averageArea + deltaArea;
				const bool bArea = area >= minArea && area <= maxArea;

				if (bMiddle && bArea)
				{
					// Debug (show validate dubious char)
					if (debug > 2)
					{
						cv::rectangle(temp, *itDubiousChar, cv::Scalar(0, 255, 0), 2);

						showImage(temp);
					}
					// If valid save it and erase it from dubious list to save computing time
					validCharacters.push_back(*itDubiousChar);
					itDubiousChar = dubiousCharacters.erase(itDubiousChar);
					continue;
				}
				else
				{
					// Debug (show invalid dubious char)
					if (debug > 2)
					{
						cv::rectangle(temp, *itDubiousChar, cv::Scalar(0, 0, 255), 2);

						showImage(temp);
					}

					// When no filter pass will be processed again, we can safely delete every character which coordinates are above the line.
					// They can only be noise so we can delete them to speed up computing time
					//if(middleY < lineData.averageY && !bMiddle && speedUp)
					if (middleY < lineData.averageY && speedUp)
					{
						itDubiousChar = dubiousCharacters.erase(itDubiousChar);
						continue;
					}
				}
			}
			++itDubiousChar;
		}
	}

	// Debug (show valid and newly validated characters)
	if (debug)
	{
		temp = dbgImage.clone();
		for (auto & character : validCharacters)
		{
			cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 2);
		}
		for (auto & line : textLines)
		{
			for (auto & character : line)
			{
				cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 1);
			}
		}
		showImage(temp);
	}

	return validCharacters;
}

filter::algos::IDPlate::LineData filter::algos::IDPlate::extractLineData(const std::vector<cv::Rect>& line)
{
	LineData lineData;
	lineData.minWidth = lineData.minHeight = std::numeric_limits<int>::max();
	lineData.maxWidth = lineData.maxHeight = std::numeric_limits<int>::min();
	lineData.minArea = std::numeric_limits<double>::max();
	lineData.maxArea = std::numeric_limits<double>::min();

	for (auto & character : line)
	{
		lineData.averageY += character.y + character.height / 2.0;
		lineData.averageCharWidth += character.width;
		lineData.averageCharHeight += character.height;

		double area = character.width * character.height;
		lineData.averageArea += area;

		if (character.width < lineData.minWidth)	lineData.minWidth = character.width;
		if (character.width > lineData.maxWidth)	lineData.maxWidth = character.width;
		if (character.height < lineData.minHeight)	lineData.minHeight = character.height;
		if (character.height > lineData.maxHeight)	lineData.maxHeight = character.height;

		if (area > lineData.maxArea)	lineData.maxArea = area;
		if (area < lineData.minArea)	lineData.minArea = area;
	}

	lineData.averageY /= line.size();
	lineData.averageCharWidth /= line.size();
	lineData.averageCharHeight /= line.size();
	lineData.averageArea /= line.size();

	return lineData;
}

cv::Mat filter::algos::IDPlate::convertColor2Gray(const cv::Mat & colorImage)
{
	cv::Mat output;
	if (colorImage.channels() == 4)
	{
		std::cout << "[WARNING] IDplateIdentifier::convertColor2Gray - Alpha is not handled";
	}

	if (colorImage.channels() == 3)
	{
		cv::cvtColor(colorImage, output, CV_BGR2GRAY);
	}
	else
	{
		output = colorImage.clone();
	}

	return output;
}

cv::Mat filter::algos::IDPlate::convertGray2Color(const cv::Mat & grayImage)
{
	cv::Mat output;
	if (grayImage.channels() == 2)
	{
		std::cout << "[WARNING] IDplateIdentifier::convertGray2Color - Alpha is not handled";
	}

	if (grayImage.channels() == 1)
	{
		cv::cvtColor(grayImage, output, CV_GRAY2BGR);
	}
	else
	{
		output = grayImage.clone();
	}
	return output;
}

cv::Mat filter::algos::IDPlate::downscaleImage(const cv::Mat & image, int downscaleRatio)
{
	if (!downscaleRatio > 1)	throw HipeException("IDPlateTools::downscaleImage - downscale ratio must be positive");

	const cv::Size newSize(image.cols / downscaleRatio, image.rows / downscaleRatio);

	cv::Mat output;
	cv::resize(image, output, newSize, 0.0, 0.0, cv::INTER_CUBIC);

	return output;
}

void filter::algos::IDPlate::showImage(const cv::Mat & image, std::string name, bool shouldDestroy, int waitTime)
{
	cv::namedWindow(name);
	cv::imshow(name, image);
	if (waitTime >= 0)	cv::waitKey(waitTime);
	if (shouldDestroy)	cv::destroyWindow(name);
}

