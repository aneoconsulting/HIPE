#include "filter\Algos\IDPlateRectifier.h"

HipeStatus filter::algos::IDPlateRectifier::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	// Debug: searchlines limits
	normalizeRatios();

	// Find ID plate's characters
	std::vector<cv::Rect> plateCharacters = findPlateCharacters(image);

	// Separate them by rows (lines)
	std::vector<int> characterRows = separateTextRows(plateCharacters);

	// Add line to bottom of image (we want rows separated by top and bottom lines)
	characterRows.push_back(image.rows - 1);

	//Debug
	if (debug_)
	{
		cv::Mat debugImage = image.clone();
		for (auto & row : characterRows)
		{
			cv::line(debugImage, cv::Point(0, row), cv::Point(debugImage.cols - 1, row), cv::Scalar(0, 0, 255), 4);
		}
		showImage(debugImage);
	}

	// Sort characters by rows
	std::vector<std::vector<cv::Rect>> charactersSorted = sortCharactersByRows(image, plateCharacters, characterRows);

	// Find whole text area
	std::vector<cv::Point> bounds = findCharactersBounds(image, charactersSorted);

	// Then extract it
	cv::Mat rectifiedPlate = perspectiveCrop(image, bounds);

	// Debug
	showImage(rectifiedPlate);

	_connexData.push(data::ImageData(rectifiedPlate));
	return OK;
}

bool filter::algos::IDPlateRectifier::compareRectsByPosition(const cv::Rect & a, const cv::Rect & b)
{
	int ay = (a.y * 0.5);
	int ax = (a.x * 0.5);
	int by = (b.y * 0.5);
	int bx = (b.x * 0.5);

	if (abs(ay - by) > 10)
		return (ay < by);

	return (ax < bx);
}

bool filter::algos::IDPlateRectifier::compareByDeriv(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.first > b.first;
}

bool filter::algos::IDPlateRectifier::compareRectByHorizontalPosition(const cv::Rect & a, const cv::Rect & b)
{
	return a.x < b.x;
}

std::vector<cv::Rect> filter::algos::IDPlateRectifier::findPlateCharacters(const cv::Mat & plateImage)
{
	cv::Mat imageCopy = plateImage.clone();
	cv::Mat imageGrayscale = convertColorToGrayscale(imageCopy);

	cv::Mat debugImage = convertGrayscaleToBGR(imageGrayscale);

	//// Preprocess image
	// First morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));
	// Debug
	showImage(imageGrayscale);

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	// Debug
	showImage(imageGrayscale);

	// Second morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));
	//Debug
	showImage(imageGrayscale);

	// Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imageGrayscale.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	//// Find characters
	std::vector<cv::Rect> charactersRects;
	cv::Mat mask = cv::Mat::zeros(imageGrayscale.size(), CV_8UC1);

	const double lowerBound = 0.20;
	const double upperBound = 0.80;
	const cv::Size minRectSize(8, 20);
	const double maxRectWidth = 0.1 * imageGrayscale.cols;
	const double xMinPos = 0.1 * imageGrayscale.cols;
	const double xMaxPos = 0.8 * imageGrayscale.cols;

	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect characterRect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, characterRect);
		maskROI = cv::Scalar(0, 0, 0);

		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);

		//// Debug (low level)
		//showImage(mask);

		// ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		//// Debug (low level)
		//if (debug_)
		//{
		//	cv::rectangle(debugImage, characterRect, cv::Scalar(255, 0, 0), 2);
		//	showImage(debugImage);
		//}

		// Assume that at least 45% of the rect area is filled if it contains text
		if (ratio > lowerBound && ratio < upperBound
			&&
			// Constraints on rect size
			(characterRect.height > minRectSize.height && characterRect.width > minRectSize.width)
			// These two conditions alone are not very robust. better to use something
			// like the number of significant peaks in a horizontal projection as a third condition
			&&
			characterRect.width <= maxRectWidth
			&&
			// Bug when the image is already cropped
			characterRect.x >= xMinPos
			&&
			characterRect.x <= xMaxPos
			)
		{
			charactersRects.push_back(characterRect);

			// Debug (low level)
			if (debug_)
			{
				cv::rectangle(debugImage, characterRect, cv::Scalar(0, 255, 0), 2);
			}
		}

		else
		{
			// Debug (low level)
			if (debug_)
			{
				cv::rectangle(debugImage, characterRect, cv::Scalar(0, 0, 255), 2);
			}
		}
		
		// Debug (low level)
		//showImage(debugImage);
	}

	// Debug
	showImage(debugImage);

	// Sort found characters by position
	std::sort(charactersRects.begin(), charactersRects.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compareRectsByPosition(a, b); });

	return charactersRects;
}

std::vector<int> filter::algos::IDPlateRectifier::separateTextRows(const std::vector<cv::Rect>& charactersRects)
{
	std::vector<int> lines;

	std::vector<std::pair<int, int>> deriv(charactersRects.size());
	std::vector<std::pair<int, int>> deriv2(charactersRects.size());

	deriv[0].first = 0;
	deriv[0].second = 0;

	int mean = 0;
	const double inclusionRatio = 0.9;

	for (int i = 1; i < charactersRects.size(); i++)
	{
		//Ignore False EOL (i.e : do not consider letter perspective) Ignore Inclusion of letter in 90% of other
		if ((charactersRects[i - 1].y >= charactersRects[i].y && charactersRects[i - 1].y <= charactersRects[i].y + inclusionRatio * charactersRects[i].height) ||
			(charactersRects[i].y >= charactersRects[i - 1].y && charactersRects[i].y <= charactersRects[i - 1].y + inclusionRatio * charactersRects[i - 1].height))
			continue;

		deriv[i - 1].first = abs(charactersRects[i - 1].y - charactersRects[i].y);
		deriv[i - 1].second = i - 1;

		mean += deriv[i - 1].first;
	}

	mean /= charactersRects.size();

	std::sort(deriv.begin(), deriv.end(), [this](const std::pair<int, int>& a, const std::pair<int, int>& b) { return this->compareByDeriv(a, b); });

	for (int i = 0; i < deriv.size(); i++)
	{
		if (deriv[i].first > 3)
		{
			int y = charactersRects[deriv[i].second].y + charactersRects[deriv[i].second].height;
			lines.push_back(y);

		}
	}

	std::sort(lines.begin(), lines.end());

	return lines;
}

std::vector<std::vector<cv::Rect>> filter::algos::IDPlateRectifier::sortCharactersByRows(const cv::Mat & plateImage, const std::vector<cv::Rect>& plateCharacters, const std::vector<int>& charactersRows)
{

	const int lines = charactersRows.size();
	std::vector<std::vector<cv::Rect>> rows(lines);

	// Debug
	cv::Mat image;
	if (debug_)
	{
		image = plateImage.clone();
		for (int i = 0; i < lines; i++)
		{
			cv::line(image, cv::Point(0, charactersRows[i]), cv::Point(image.cols - 1, charactersRows[i]), cv::Scalar(0, 0, 255));
		}
		showImage(image);
	}


	// For each line and the next one
	for (int j = 0, prevLine = 0, nextLine = charactersRows[j]; j < lines; j++)
	{
		nextLine = charactersRows[j];

		// Debug
		if (debug_)
		{
			image = plateImage.clone();
		}

		// For each character : if character is between the two lines add it to the correct container
		for (int i = 0; i < plateCharacters.size(); i++)
		{
			cv::Point rectCenter;
			rectCenter.x = plateCharacters[i].x + plateCharacters[i].width * 0.5;
			rectCenter.y = plateCharacters[i].y + plateCharacters[i].height * 0.5;

			// Debug
			if (debug_)
			{
				cv::circle(image, rectCenter, 4, cv::Scalar(255, 255, 0), 4);
				//showImage(image);
			}

			if (rectCenter.y > prevLine && rectCenter.y < nextLine)
			{
				rows[j].push_back(plateCharacters[i]);
			}
		}

		prevLine = nextLine;
	}

	return rows;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findCharactersBounds(const cv::Mat & image, const std::vector<std::vector<cv::Rect>>& charactersSorted)
{
	// Extract the 3 characters lines with the longest characters chain
	std::vector<std::vector<cv::Rect>> bestLines = findLongestTextLines(3, charactersSorted);

	// Sort the rects in the lines by position
	for (auto & line : bestLines)
	{
		std::sort(line.begin(), line.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compareRectByHorizontalPosition(a, b); });
	}

	// Find the lines bounds
	std::vector<cv::Point> charactersBounds = findCharactersLinesBounds(bestLines, image);

	// Find the whole text area bounds
	std::vector<cv::Point> charactersAreaBounds = findPlateTextArea(image, charactersBounds);

	return charactersAreaBounds;
}

std::vector<std::vector<cv::Rect>> filter::algos::IDPlateRectifier::findLongestTextLines(int linesToFind, const std::vector<std::vector<cv::Rect>>& textList)
{
	std::vector<std::vector<cv::Rect>> longestLines(linesToFind);
	std::vector<std::pair<int, double>> nbDeriv(textList.size());

	// Filter lines : Min number of characters to take into account
	const int countMinCharacters = 3;

	for (int i = 0; i < textList.size(); i++)
	{
		if (textList[i].size() < countMinCharacters) continue;

		std::vector<double> deriv(textList[i].size() - 1);
		std::vector<double> deriv2(deriv.size() - 1);

		double derivMean = 0.0;
		for (int j = 0; j < textList[i].size() - 1; j++)
		{
			deriv[j] = abs(textList[i][j + 1].y - textList[i][j].y);

			if (j == 0) continue;

			derivMean += abs(deriv[j] - deriv[j - 1]);
		}

		derivMean /= textList[i].size();
		nbDeriv[i].first = textList[i].size();
		nbDeriv[i].second = derivMean;
	}

	int bestIndex = -1;
	int bestSumDeriv2 = std::numeric_limits<int>::max();

	//Select best index by deriv second 
	for (int i = 0; i < nbDeriv.size() - linesToFind + 1; i++)
	{
		double deriv2Mean = 0.0;

		if (nbDeriv[i].first == 0 || nbDeriv[i].second == 0.0)
			continue;
		if (i + linesToFind > nbDeriv.size())
			break;
		bool isEmpty = false;

		for (int j = 0; j < linesToFind; j++)
		{
			if (nbDeriv[i + j].first == 0 || nbDeriv[i + j].second == 0.0)
			{
				isEmpty = true;
				break;
			}


			deriv2Mean += nbDeriv[i + j].second;
		}
		if (isEmpty) continue;

		deriv2Mean /= static_cast<double>(linesToFind);
		if (deriv2Mean < bestSumDeriv2)
		{
			bestSumDeriv2 = deriv2Mean;
			bestIndex = i;
		}
	}

	for (int j = 0; j < linesToFind; j++) {
		longestLines[j] = textList[bestIndex + j];
	}

	return longestLines;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findCharactersLinesBounds(const std::vector<std::vector<cv::Rect>> lines, const cv::Mat & image)
{
	// Debug
	bool debug = image.data ? true : false;

	int xMinLeft = std::numeric_limits<int>::max();
	int yMinLeft = std::numeric_limits<int>::max();

	int xMaxLeft = std::numeric_limits<int>::min();
	int yMaxLeft = std::numeric_limits<int>::min();

	int xMinRight = std::numeric_limits<int>::max();
	int yMinRight = std::numeric_limits<int>::max();

	int xMaxRight = std::numeric_limits<int>::min();
	int yMaxRight = std::numeric_limits<int>::min();

	// Find characters englobing 4 corners (min/max left x/y coords min/max right x/y coords)
	for (auto & line : lines)
	{
		const cv::Rect& firstCharacter = line.front();
		if (xMinLeft > firstCharacter.x)							xMinLeft = firstCharacter.x;
		if (xMaxLeft < firstCharacter.x + firstCharacter.width)		xMaxLeft = firstCharacter.x + firstCharacter.width;
		if (yMinLeft > firstCharacter.y)							yMinLeft = firstCharacter.y;
		if (yMaxLeft < firstCharacter.y + firstCharacter.height)	yMaxLeft = firstCharacter.y + firstCharacter.height;

		const cv::Rect& lastCharacter = line.back();
		if (xMinRight > lastCharacter.x)							xMinRight = lastCharacter.x;
		if (xMaxRight < lastCharacter.x + lastCharacter.width)		xMaxRight = lastCharacter.x + lastCharacter.width;
		if (yMinRight > lastCharacter.y)							yMinRight = lastCharacter.y;
		if (yMaxRight < lastCharacter.y + lastCharacter.height)		yMaxRight = lastCharacter.y + lastCharacter.height;


		if (debug && debug_)
		{
			cv::Mat temp = image.clone();
			cv::rectangle(temp, firstCharacter, cv::Scalar(255, 0, 0));
			cv::rectangle(temp, lastCharacter, cv::Scalar(255, 0, 0));
			cv::circle(temp, cv::Point(firstCharacter.x + firstCharacter.width, firstCharacter.y + firstCharacter.height), 4, cv::Scalar(0, 0, 255), 2);
			cv::circle(temp, cv::Point(lastCharacter.x + lastCharacter.width, lastCharacter.y + lastCharacter.height), 4, cv::Scalar(0, 0, 255), 2);


			cv::circle(temp, cv::Point(xMinLeft, yMinLeft), 2, cv::Scalar(0, 255, 0), 2);
			cv::circle(temp, cv::Point(xMaxLeft, yMaxLeft), 2, cv::Scalar(0, 255, 0), 2);
			cv::circle(temp, cv::Point(xMinRight, yMinRight), 2, cv::Scalar(0, 255, 0), 2);
			cv::circle(temp, cv::Point(xMaxRight, yMaxRight), 2, cv::Scalar(0, 255, 0), 2);

			cv::line(temp, cv::Point(0, yMinLeft), cv::Point(temp.cols, yMinLeft), cv::Scalar(255, 0, 255));
			cv::line(temp, cv::Point(0, yMaxLeft), cv::Point(temp.cols, yMaxLeft), cv::Scalar(255, 0, 255));
			cv::line(temp, cv::Point(0, yMinRight), cv::Point(temp.cols, yMinRight), cv::Scalar(255, 0, 255));
			cv::line(temp, cv::Point(0, yMaxRight), cv::Point(temp.cols, yMaxRight), cv::Scalar(255, 0, 255));
			showImage(temp);
		}
	}

	cv::Point topLeft(xMinLeft, yMinLeft);
	cv::Point topRight(xMaxRight, yMinRight);

	if (debug && debug_)
	{
		cv::Mat temp = image.clone();
		cv::line(temp, topLeft, topRight, cv::Scalar(255, 255, 0), 4);
		showImage(temp);
	}

	// To compute the bottom corners we must take into account the homography perspective
	// Compute the bottom line slope
	int yRight = lines.back().back().y + lines.back().back().height;
	int yLeft = lines.back().front().y + lines.back().front().height;

	int xRight = lines.back().back().x;
	int xLeft = lines.back().front().x;

	double a = (yRight - yLeft) / (double)(xRight - xLeft);
	double b = yRight - a  * xRight;

	double finalBottomRightY = a * xMaxRight + b;
	double finalBottomLeftY = a * xMinLeft + b;

	cv::Point bottomRight(xMaxRight, finalBottomRightY);
	cv::Point bottomLeft(xMinLeft, finalBottomLeftY);

	if (debug)
	{
		cv::Mat temp = image.clone();
		cv::circle(temp, topLeft, 4, cv::Scalar(255, 0, 255), 4);
		cv::circle(temp, topRight, 4, cv::Scalar(255, 0, 255), 4);
		cv::circle(temp, bottomLeft, 4, cv::Scalar(255, 0, 255), 4);
		cv::circle(temp, bottomRight, 4, cv::Scalar(255, 0, 255), 4);

		cv::line(temp, topLeft, topRight, cv::Scalar(0, 0, 255), 3);
		cv::line(temp, topRight, bottomRight, cv::Scalar(0, 0, 255), 3);
		cv::line(temp, bottomRight, bottomLeft, cv::Scalar(0, 0, 255), 3);
		cv::line(temp, bottomLeft, topLeft, cv::Scalar(0, 0, 255), 3);

		showImage(temp);
	}


	std::vector<cv::Point> output;
	output.push_back(topLeft);
	output.push_back(topRight);
	output.push_back(bottomRight);
	output.push_back(bottomLeft);

	return output;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findPlateTextArea(const cv::Mat & plateImage, const std::vector<cv::Point>& textCorners)
{
	cv::Mat image = convertColorToGrayscale(plateImage);

	std::vector<cv::Point> newCorners(textCorners);

	//Preprocess image (binary & bigger lines)
	cv::Mat binaryImage;
	cv::Canny(image, binaryImage, 100, 50, 3, true);
	binaryImage = applyMorphTransform(binaryImage, cv::MorphShapes::MORPH_CROSS, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));

	// Extract the image's Horizontal lines
	cv::Mat horizontalLines;
	const int horizontalFactor = 30;
	const int horizontalSize = image.cols / horizontalFactor;

	cv::Mat horizontalKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(horizontalFactor, 1));
	cv::erode(binaryImage, horizontalLines, horizontalKernel);
	cv::dilate(horizontalLines, horizontalLines, horizontalKernel);
	cv::dilate(horizontalLines, horizontalLines, cv::Mat::ones(3, 3, CV_8UC1));
	// Debug
	showImage(horizontalLines);

	// Find top line
	std::vector<cv::Point> topCorners = findAreaTopLine(horizontalLines, textCorners);
	newCorners[0] = topCorners[0];
	newCorners[1] = topCorners[1];

	// Extract the image's vertical lines
	cv::Mat verticalLines;
	const int verticalFactor = image.rows / 30;
	cv::Mat verticalKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(1, verticalFactor));
	cv::erode(binaryImage, verticalLines, verticalKernel);
	cv::dilate(verticalLines, verticalLines, verticalKernel);
	// Debug
	showImage(verticalLines);

	std::vector<cv::Point> leftCorners = findAreaLeftLine(verticalLines, newCorners);
	newCorners[0] = leftCorners[0];
	newCorners[3] = leftCorners[1];

	std::vector<cv::Point> rightCorners = findAreaRightLine(verticalLines, newCorners);
	newCorners[1] = rightCorners[0];
	newCorners[2] = rightCorners[1];

	// Debug
	if (debug_)
	{
		cv::Mat newTextArea = plateImage.clone();
		cv::circle(newTextArea, newCorners[0], 3, cv::Scalar(255, 0, 0), 2);
		cv::circle(newTextArea, newCorners[1], 3, cv::Scalar(255, 0, 0), 2);
		cv::circle(newTextArea, newCorners[2], 3, cv::Scalar(255, 0, 0), 2);
		cv::circle(newTextArea, newCorners[3], 3, cv::Scalar(255, 0, 0), 2);

		cv::line(newTextArea, newCorners[0], newCorners[1], cv::Scalar(255, 0, 255), 2);
		cv::line(newTextArea, newCorners[1], newCorners[2], cv::Scalar(255, 0, 255), 2);
		cv::line(newTextArea, newCorners[2], newCorners[3], cv::Scalar(255, 0, 255), 2);
		cv::line(newTextArea, newCorners[3], newCorners[0], cv::Scalar(255, 0, 255), 2);

		showImage(newTextArea);
	}
	return newCorners;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findAreaTopLine(const cv::Mat & plateHorizontalLines, const std::vector<cv::Point>& textCorners)
{
	cv::Point2f pt1 = textCorners[0];
	cv::Point2f pt2 = textCorners[1];
	cv::Point2f ptNext;

	SearchDirection searchDirection = SearchDirection::UP;

	// TODO: WARNING: BE CAREFUL HERE: what if pt1.x == pt2.x ?
	int xmin = cv::min(pt1.x, pt2.x);
	int xmax = cv::max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// Be sure the points are ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// Move a bit the on the y axis to stick on vertical line
	pt1.x = cv::min(plateHorizontalLines.cols - 1, (int)(pt1.x + abs(xmax - xmin) / 4.0)); // 4 is 1/4 of a horizontal line
	pt2.x = cv::max(0, (int)(pt2.x /*- abs(xmax - xmin) / 4.0*/));

	// Debug
	if (debug_)
	{
		cv::Mat searchLine = convertGrayscaleToBGR(plateHorizontalLines);
		cv::line(searchLine, cv::Point(pt1.x, 0), pt1, cv::Scalar(0, 0, 255), 1);
		cv::line(searchLine, cv::Point(pt2.x, 0), pt2, cv::Scalar(0, 0, 255), 1);
		cv::circle(searchLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(searchLine, pt2, 3, cv::Scalar(255, 255, 0), 3);
		showImage(searchLine);
	}

	ptNext.x = pt1.x;
	ptNext.y = pt1.y;
	int maxValue = 0;
	cv::Point cartesianMax(-1, -1);

	double rho = sqrt(square(pt2.x - pt1.x) + square(pt2.y - pt2.y));

	cv::Point2f limit;

	limit.y = (cv::max(0.0, (double)(pt1.y - topRatio * plateHorizontalLines.rows)));
	limit.x = pt1.x; // ignore the limit on x for top line research

	// Search longest horizontal line from the characters englobing rect corners found earlier
	while (ptNext.y >= 0)
	{
		// Find pivot point (white pixel possibly on the longest line)
		cv::Point2f point = ptNext = _findNextTopPivotPoint(plateHorizontalLines, ptNext, limit);
		if (ptNext.x == -1) break;

		double theta = 0.0;
		cv::Vec2f bestAlign;


		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		// From the found pivot point, find the other end of the line
		int currBest = findBestHorizontalLine(plateHorizontalLines, ptNext, cv::Vec2f(rho, theta), bestAlign);

		if (maxValue < currBest)
		{
			maxValue = currBest;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesianMax.x = ptNext.x + rho * cos(bestAlign[1]);
			cartesianMax.y = ptNext.y + rho * sin(bestAlign[1]);
		}
	}

	// Debug
	if (debug_)
	{
		cv::Mat topLine = convertGrayscaleToBGR(plateHorizontalLines);
		cv::line(topLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 1);
		cv::circle(topLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(topLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
		showImage(topLine);
	}

	// Now compute new corners and lines intersection
	cv::Vec4i line1, line2;
	std::vector<cv::Point> topLineCorners;
	line1[0] = textCorners[0].x;
	line1[1] = textCorners[0].y;
	line1[2] = textCorners[3].x;
	line1[3] = textCorners[3].y;

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesianMax.x;
	line2[3] = cartesianMax.y;
	cv::Point areaTopLeftCorner = computeLinesIntersectionPoint(line1, line2);
	topLineCorners.push_back(areaTopLeftCorner);

	line1[0] = textCorners[1].x;
	line1[1] = textCorners[1].y;
	line1[2] = textCorners[2].x;
	line1[3] = textCorners[2].y;
	cv::Point areaTopRightCorner = computeLinesIntersectionPoint(line1, line2);
	topLineCorners.push_back(areaTopRightCorner);

	// Debug
	if (debug_)
	{
		cv::Mat topLine = convertGrayscaleToBGR(plateHorizontalLines);
		cv::line(topLine, areaTopLeftCorner, areaTopRightCorner, cv::Scalar(0, 0, 255), 1);
		cv::circle(topLine, areaTopLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(topLine, areaTopRightCorner, 3, cv::Scalar(255, 255, 0), 3);
		showImage(topLine);
	}

	return topLineCorners;
}

cv::Point filter::algos::IDPlateRectifier::_findNextTopPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin;
	int end;

	int chan = plateImage.channels();

	cv::Mat col = plateImage.col(currPosition.x).clone();
	cv::Mat tCol = col.t();

	// TODO: TM: Weid behavior with pointers (using .at insteand) <= Optimize
	//const uchar * _col = tCol.ptr<unsigned char>(0);
	//Now look for next line by white pixel
	for (int y = currPosition.y; y >= limit.y; y--)
	{
		// TODO: <== HERE
		const int channelValue = static_cast<int>(tCol.at<uchar>(0, y));
		//if (foundBegin == false && _col[y] == 255)	// TM: <== Weird behavior with pointers
		if (!foundBegin && channelValue == 255)
		{
			foundBegin = true;
			begin = y;
			continue;
		}

		//if (foundBegin == true && _col[y] == 0)			// TM: <== Weird behavior with pointers
		if (foundBegin && channelValue == 0)
		{
			end = cv::max(0, y - 1);

			cv::Point output(currPosition.x, cv::max(0, begin + (end - begin) / 2));

			// Debug
			
			//cv::Mat temp = col.clone();
			//cv::circle(temp, output, 4, cv::Scalar(255, 255, 0));
			//showImage(temp);

			return output;
		}
	}
	if (!foundBegin) return cv::Point(-1, -1);

	return cv::Point(currPosition.y, 0);
}

int filter::algos::IDPlateRectifier::findBestHorizontalLine(const cv::Mat & image, const cv::Point & origin, const cv::Vec2f & lineVec, cv::Vec2f & out_bestLineParameters)
{
	const double nbSampleTheta = 60;
	const double nbSampleRho = 60;
	const double minAngle = -CV_PI / 12; // (- 15 degrees)
	const double maxAngle = CV_PI / 12; // ( + 15 degrees)
	const double rho = lineVec[0];
	const double strideTheta = (maxAngle - minAngle) / nbSampleTheta;
	const double strideRho = (rho) / nbSampleRho;

	int sum = 0;
	int sumMax = 0;
	double tBest = 0.0;

	cv::Point2f bestPoint;

	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sinT = sin(t);
		double cosT = cos(t);

		for (double r = 1; r < rho; r += strideRho)
		{
			int x = cv::max(0.0, cv::min((double)image.cols - 1, origin.x + r * cosT));

			int y = cv::max(0.0, cv::min((double)image.rows - 1, origin.y + r * sinT));


			// TODO: TM: Weid behavior with pointers (using .at insteand) <= Optimize
			//const uchar * rows = image.ptr(y);
			//const int debugVAlue = rows[x];
			const int channelValue = static_cast<int>(image.at<uchar>(y, x));
			//if (rows[x] == 255)
			if (channelValue == 255.0)
			{
				sum++;
			}
		}

		// Debug
		const int debugX = origin.x + rho * cosT;
		const int debugY = origin.y + rho * sinT;
		//cv::Mat searchLine = convertGrayscaleToBGR(image);
		//cv::circle(searchLine, origin, 3, cv::Scalar(0, 0, 255), 3);
		//cv::circle(searchLine, cv::Point2f(debugX, debugY), 3, cv::Scalar(0, 0, 255), 3);
		//cv::line(searchLine, origin, cv::Point2f(debugX, debugY), cv::Scalar(0, 255, 0), 2);
		//showImage(searchLine);

		if (sumMax < sum)
		{
			sumMax = sum;
			tBest = t;

			// Debug
			bestPoint = cv::Point2f(debugX, debugY);
		}
	}

	//// Debug
	//cv::Mat temp = convertGrayscaleToBGR(image);
	//cv::circle(temp, origin, 2, cv::Scalar(255, 0, 0), 3);
	//cv::circle(temp, bestPoint, 2, cv::Scalar(255, 0, 0), 3);
	//cv::line(temp, origin, bestPoint, cv::Scalar(0, 0, 255), 4);
	//showImage(temp);

	out_bestLineParameters = cv::Vec2f(rho, tBest);

	return sumMax;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findAreaLeftLine(const cv::Mat & plateVerticalLines, const std::vector<cv::Point>& textCorners)
{
	cv::Point2f pt1 = textCorners[0];
	cv::Point2f pt2 = textCorners[3];
	cv::Point2f ptNext;

	SearchDirection searchDirection = SearchDirection::LEFT;

	// TODO: WARNING: BE CAREFUL HERE: what if pt1.x == pt2.x ?
	int xmin = cv::min(pt1.x, pt2.x);
	int xmax = cv::max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// Be sure the points are ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// Move a bit the on the y axis to stick on vertical line
	pt1.y = cv::min(plateVerticalLines.rows - 1, (int)(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = cv::max(0, (int)(pt2.y - abs(ymax - ymin) / 6.0));				// 6 is the middle of 3 lines of text


																			// Debug
	cv::Mat searchLine = convertGrayscaleToBGR(plateVerticalLines);
	cv::line(searchLine, cv::Point(0, pt1.y), pt1, cv::Scalar(0, 0, 255), 1);
	cv::line(searchLine, cv::Point(0, pt2.y), pt2, cv::Scalar(0, 0, 255), 1);
	cv::circle(searchLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
	cv::circle(searchLine, pt2, 3, cv::Scalar(255, 255, 0), 3);
	showImage(searchLine);

	ptNext.x = pt1.x;
	ptNext.y = pt1.y;
	int maxValue = 0;
	cv::Point cartesianMax(-1, -1);
	double rho = sqrt(square(pt2.x - pt1.x) + square(pt2.y - pt1.y));

	cv::Point2f limit;
	limit.x = (cv::max(0.0, (double)(pt1.x - leftRatio * plateVerticalLines.cols)));
	limit.y = pt1.y; // ignore the limit on x for top line research

	while (ptNext.x > 0)
	{
		ptNext = _findNextLeftPivotPoint(plateVerticalLines, ptNext, limit);
		//After avoid to shift on a horizontal line a bad rows, we set to the orignal value of row

		if (ptNext.x == -1) break;


		int xp = xmax - ptNext.x;
		int yp = ymax - ptNext.y;

		double theta = atan2(xp, yp);

		cv::Vec2f bestAlign;


		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		int currBest = findBestVerticalLine(plateVerticalLines, ptNext, cv::Vec2f(rho, theta), bestAlign);

		if (maxValue < currBest)
		{
			maxValue = currBest;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesianMax.x = ptNext.x + rho * cos(bestAlign[1]);
			cartesianMax.y = ptNext.y + rho * sin(bestAlign[1]);
		}

	}

	// Debug
	cv::Mat leftLine = convertGrayscaleToBGR(plateVerticalLines);
	cv::line(leftLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 2);
	cv::circle(leftLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
	cv::circle(leftLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
	showImage(leftLine);

	//Now compute corner and the new intersection
	cv::Vec4i line1, line2;
	std::vector<cv::Point> LeftCorners;
	line1[0] = textCorners[0].x;
	line1[1] = textCorners[0].y;
	line1[2] = textCorners[1].x;
	line1[3] = textCorners[1].y;

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesianMax.x;
	line2[3] = cartesianMax.y;
	cv::Point areaTopLeftCorner = computeLinesIntersectionPoint(line1, line2);
	LeftCorners.push_back(areaTopLeftCorner);

	line1[0] = textCorners[2].x;
	line1[1] = textCorners[2].y;
	line1[2] = textCorners[3].x;
	line1[3] = textCorners[3].y;
	cv::Point areaBottomLeftCorner = computeLinesIntersectionPoint(line1, line2);
	LeftCorners.push_back(areaBottomLeftCorner);

	// Debug
	leftLine = IDPlateRectifier::convertGrayscaleToBGR(plateVerticalLines);
	cv::line(leftLine, areaTopLeftCorner, areaBottomLeftCorner, cv::Scalar(0, 0, 255), 1);
	cv::circle(leftLine, areaTopLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
	cv::circle(leftLine, areaBottomLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
	showImage(leftLine);

	cv::Mat upperLeftCorner = leftLine;
	cv::line(upperLeftCorner, areaTopLeftCorner, textCorners[1], cv::Scalar(0, 0, 255), 1);
	cv::circle(upperLeftCorner, textCorners[1], 3, cv::Scalar(255, 255, 0), 3);
	IDPlateRectifier::showImage(upperLeftCorner);

	return LeftCorners;
}

cv::Point filter::algos::IDPlateRectifier::_findNextLeftPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin, end;

	int chan = plateImage.channels();

	// TODO: TM: Weid behavior with pointers (using .at insteand) <= Optimize
	//const uchar* row = image.ptr(currPosition.y);

	//Now look for next line by white pixel
	// TM: >= replaced by >
	for (int x = currPosition.x - 1; x > limit.x; x--)
	{
		//// TODO: <== HERE
		//// Debug
		//cv::Mat temp = IDPlateRectifier::convertGrayscaleToBGR(plateImage);
		//cv::Point currentPos(x, currPosition.y);
		//cv::circle(temp, currentPos, 2, cv::Scalar(0, 0, 255), 2);
		//IDPlateRectifier::showImage(temp);

		const int channelValue = static_cast<int>(plateImage.at<uchar>(currPosition.y, x));
		//if (found_begin == false && row[x] == 255)	// TM: <== Weird behavior with pointers
		if (!foundBegin && channelValue == 255)
		{
			foundBegin = true;
			begin = x;
			continue;
		}

		//if (foundBegin && row[x] == 0)
		if (foundBegin && channelValue == 0)
		{
			end = x;
			cv::Point output(cv::max(0, end + 1 + (begin - end) / 2), currPosition.y);

			//// Debug
			//cv::Mat temp = IDPlateRectifier::convertGrayscaleToBGR(plateImage);
			//cv::circle(temp, output, 4, cv::Scalar(255, 255, 0));
			//IDPlateRectifier::showImage(temp);
			return output;
		}
	}

	if (!foundBegin) return cv::Point(-1, -1);

	// TM
	else
	{
		//cv::Point output(cv::max(0, limit.x + 1 + (begin - limit.x) / 2), currPosition.y);	// OLD
		cv::Point output(cv::max(0, limit.x + abs(begin - limit.x) / 2), currPosition.y);
		return output;
	}
}

int filter::algos::IDPlateRectifier::findBestVerticalLine(const cv::Mat & image, const cv::Point & origin, const cv::Vec2f & lineVec, cv::Vec2f & out_bestLineParameters)
{
	const double nbSampleTheta = 100;
	const double nbSampleRho = 60;
	const double minAngle = CV_PI / 2 - CV_PI / 12; // (- 15 degrees)
	const double maxAngle = CV_PI / 2 + CV_PI / 12; // ( + 15 degrees)
	const double rho = lineVec[0];
	const double strideTheta = (maxAngle - minAngle) / nbSampleTheta;
	const double strideRho = (rho / nbSampleRho) < 1.0 ? 1.0 : (rho / nbSampleRho);

	int sum;
	int maxSum = 0;
	double tBest = 0.0;
	cv::Point2f bestPoint;

	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sinT = sin(t);
		double cosT = cos(t);

		for (double r = 0.0; r < rho; r += strideRho)
		{
			int x = cv::min(image.cols - 1, (int)(origin.x + r * cosT + 0.5));
			x = cv::max(0, x);

			int y = cv::min(image.rows - 1, (int)(origin.y + r * sinT + 0.5));
			y = cv::max(0, y);

			//// Debug
			//if (origin.x == 28 || origin.y == 28)
			//{
				//cv::Mat searchLine = convertGrayscaleToBGR(image);
				//cv::circle(searchLine, origin, 3, cv::Scalar(255, 0, 255), 2);
				//cv::circle(searchLine, cv::Point(x, y), 3, cv::Scalar(255, 0, 255), 2);
				//cv::line(searchLine, origin, cv::Point(x, y), cv::Scalar(0, 255, 0, 4));
				//showImage(searchLine);
			//}
			//const uchar * rows = image.ptr(y);
			const int channelValue = static_cast<int>(image.at<uchar>(y, x));
			//if (rows[x] == 255)
			if (channelValue == 255)
			{
				sum++;
			}
		}

		// Debug
		double debugX = origin.x + rho * cosT + 0.5;
		double debugY = origin.y + rho * sinT + 0.5;
		//cv::Mat searchLine = convertGrayscaleToBGR(image);
		//cv::circle(searchLine, origin, 3, cv::Scalar(0, 0, 255), 3);
		//cv::circle(searchLine, cv::Point2f(debugX, debugY), 3, cv::Scalar(0, 0, 255), 3);
		//cv::line(searchLine, origin, cv::Point2f(debugX, debugY), cv::Scalar(0, 255, 0), 2);
		//showImage(searchLine);


		if (maxSum < sum)
		{
			maxSum = sum;
			tBest = t;

			// Debug
			bestPoint = cv::Point2f(debugX, debugY);
		}
	}

	//// Debug
	//cv::Mat temp = IDPlateRectifier::convertGrayscaleToBGR(image);
	//cv::circle(temp, origin, 2, cv::Scalar(255, 0, 0), 3);
	//cv::circle(temp, bestPoint, 2, cv::Scalar(255, 0, 0), 3);
	//cv::line(temp, origin, bestPoint, cv::Scalar(0, 0, 255), 4);
	//IDPlateRectifier::showImage(temp);

	out_bestLineParameters = cv::Vec2f(rho, tBest);

	return maxSum;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findAreaRightLine(const cv::Mat & plateVerticalLines, const std::vector<cv::Point>& textCorners)
{
	cv::Point2f pt1 = textCorners[1];
	cv::Point2f pt2 = textCorners[2];
	cv::Point2f ptNext;
	IDPlateRectifier::SearchDirection searchDirection = IDPlateRectifier::SearchDirection::RIGHT;

	// TODO: WARNING: BE CAREFUL HERE: what if pt1.x == pt2.x ?
	int xmin = cv::min(pt1.x, pt2.x);
	int xmax = cv::max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// be sure the point are ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// Move a bit the on the y axis to stick on vertical line
	pt1.y = cv::min(plateVerticalLines.rows - 1, (int)(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = cv::max(0, (int)(pt2.y - abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	ptNext.x = pt1.x;
	ptNext.y = pt1.y;

	int max = 0;
	cv::Point cartesianMax(-1, -1);

	double rho = sqrt(square(xmax - xmin) + square(ymax - ymin));

	cv::Point2f limit;
	limit.x = (cv::min(static_cast<double>(pt1.x + rightRatio * plateVerticalLines.cols), static_cast<double>(plateVerticalLines.cols - 1)));
	limit.y = pt1.y; // ignore the limit on x for top line research

	while (ptNext.x < plateVerticalLines.cols)
	{
		ptNext = _findNextRightPivotPoint(plateVerticalLines, ptNext, limit);
		//After avoid to shift on a horizontal line a bad rows, we set to the orignal value of row

		if (ptNext.x == -1) break;


		double theta = 0.0;

		cv::Vec2f bestAlign;


		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		//int current_best = bestlineStickV(input, ptNext, Vec2f(rho, theta), best_align);
		int currentBest = findBestVerticalLine(plateVerticalLines, ptNext, cv::Vec2f(rho, theta), bestAlign);

		if (max < currentBest)
		{
			max = currentBest;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesianMax.x = ptNext.x + rho * cos(bestAlign[1]);
			cartesianMax.y = ptNext.y + rho * sin(bestAlign[1]);
		}
	}

	// Debug
	cv::Mat rightLine = convertGrayscaleToBGR(plateVerticalLines);
	cv::line(rightLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 2);
	cv::circle(rightLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
	cv::circle(rightLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
	showImage(rightLine);

	//Now compute corner and the new intersection
	cv::Vec4i line1, line2;
	std::vector<cv::Point> rightCorners;
	line1[0] = textCorners[0].x;
	line1[1] = textCorners[0].y;
	line1[2] = textCorners[1].x;
	line1[3] = textCorners[1].y;

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesianMax.x;
	line2[3] = cartesianMax.y;
	cv::Point areaTopRightCorner = computeLinesIntersectionPoint(line1, line2);
	rightCorners.push_back(areaTopRightCorner);

	line1[0] = textCorners[2].x;
	line1[1] = textCorners[2].y;
	line1[2] = textCorners[3].x;
	line1[3] = textCorners[3].y;
	cv::Point areaBottomRightCorner = computeLinesIntersectionPoint(line1, line2);
	rightCorners.push_back(areaBottomRightCorner);

	// Debug
	rightLine = convertGrayscaleToBGR(plateVerticalLines);
	cv::line(rightLine, areaTopRightCorner, areaBottomRightCorner, cv::Scalar(0, 0, 255), 1);
	cv::circle(rightLine, areaTopRightCorner, 3, cv::Scalar(255, 255, 0), 3);
	cv::circle(rightLine, areaBottomRightCorner, 3, cv::Scalar(255, 255, 0), 3);
	showImage(rightLine);

	return rightCorners;
}

cv::Point filter::algos::IDPlateRectifier::_findNextRightPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin, end;

	// TODO: TM: Weid behavior with pointers (using .at insteand) <= Optimize
	//const uchar* row = plateImage.ptr(currPosition.y);

	//Now look for next line by white pixel
	for (int x = currPosition.x + 1; x < limit.x; x++)
	{
		//// Debug
		//cv::Mat temp = IDPlateRectifier::convertGrayscaleToBGR(plateImage);
		//cv::Point currentPos(x, currPosition.y);
		//cv::circle(temp, currentPos, 2, cv::Scalar(0, 0, 255), 2);
		//IDPlateRectifier::showImage(temp);


		const int channelValue = static_cast<int>(plateImage.at<uchar>(currPosition.y, x));
		//if (found_begin == false && row[x] == 255)	// TM: <== Weird behavior with pointers
		if (!foundBegin && channelValue == 255)
		{
			foundBegin = true;
			begin = x;
			continue;
		}

		//if (found_begin == true && row[x] == 0)
		if (foundBegin && channelValue == 0)
		{
			end = cv::max(0, x - 1);
			cv::Point output(cv::max(0, begin + (end - begin) / 2), currPosition.y);
			//// Debug
			//cv::Mat temp = IDPlateRectifier::convertGrayscaleToBGR(plateImage);
			//cv::circle(temp, output, 4, cv::Scalar(255, 255, 0));
			//IDPlateRectifier::showImage(temp);
			return output;
		}
	}

	if (!foundBegin) return cv::Point(-1, -1);

	//return cv::Point(-1, pt.y);
	else
	{
		cv::Point output(cv::max(0, limit.x + abs(limit.x - begin) / 2), currPosition.y);
		return output;
	}
}

cv::Mat filter::algos::IDPlateRectifier::perspectiveCrop(const cv::Mat & plateImage, const std::vector<cv::Point>& textCorners)
{
	assert(plateImage.data);

	//Debug
	cv::Mat temp = plateImage.clone();
	cv::circle(temp, textCorners[0], 3, cv::Scalar(0, 0, 255), 2);
	cv::circle(temp, textCorners[1], 3, cv::Scalar(0, 0, 255), 2);
	cv::circle(temp, textCorners[2], 3, cv::Scalar(0, 0, 255), 2);
	cv::circle(temp, textCorners[3], 3, cv::Scalar(0, 0, 255), 2);

	cv::line(temp, textCorners[0], textCorners[1], cv::Scalar(0, 0, 255));
	cv::line(temp, textCorners[1], textCorners[2], cv::Scalar(0, 0, 255));
	cv::line(temp, textCorners[2], textCorners[3], cv::Scalar(0, 0, 255));
	cv::line(temp, textCorners[3], textCorners[0], cv::Scalar(0, 0, 255));
	IDPlateRectifier::showImage(temp);

	cv::Rect boundingRect = cv::boundingRect(textCorners);

	std::vector<cv::Point2f> perspectiveInput;
	perspectiveInput.push_back(cv::Point2f(textCorners[0]));
	perspectiveInput.push_back(cv::Point2f(textCorners[1]));
	perspectiveInput.push_back(cv::Point2f(textCorners[2]));
	perspectiveInput.push_back(cv::Point2f(textCorners[3]));

	std::vector<cv::Point2f> perspectiveOutput;
	perspectiveOutput.push_back(cv::Point2f(0, 0));
	perspectiveOutput.push_back(cv::Point2f(boundingRect.width, 0));
	perspectiveOutput.push_back(cv::Point2f(boundingRect.width, boundingRect.height));
	perspectiveOutput.push_back(cv::Point2f(0, boundingRect.height));

	cv::Mat output = cv::Mat::zeros(boundingRect.height, boundingRect.width, CV_8UC1);

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(perspectiveInput, perspectiveOutput);
	cv::warpPerspective(plateImage, output, perspectiveTransform, output.size());

	return output;
}

cv::Point2f filter::algos::IDPlateRectifier::computeLinesIntersectionPoint(const cv::Vec4i & a, const cv::Vec4i & b)
{
	int x1 = a[0], y1 = a[1];
	int x2 = a[2], y2 = a[3];
	int x3 = b[0], y3 = b[1];
	int x4 = b[2], y4 = b[3];

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
		pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

cv::Mat filter::algos::IDPlateRectifier::applyMorphTransform(const cv::Mat & image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize)
{
	cv::Mat output;
	cv::Mat morphKernel = cv::getStructuringElement(morphShape, kernelSize);
	cv::morphologyEx(image, output, morphType, morphKernel);

	return output;
}

void filter::algos::IDPlateRectifier::showImage(const cv::Mat & image)
{
	if (!debug_) return;
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");
}

cv::Mat filter::algos::IDPlateRectifier::convertColorToGrayscale(const cv::Mat & plateImageColor)
{
	cv::Mat output;
	(plateImageColor.channels() != 1) ? cv::cvtColor(plateImageColor, output, CV_BGR2GRAY) : output = plateImageColor.clone();

	return output;
}

cv::Mat filter::algos::IDPlateRectifier::convertGrayscaleToBGR(const cv::Mat & plateImageGrayscale)
{
	cv::Mat output;
	(plateImageGrayscale.channels() != 3) ? cv::cvtColor(plateImageGrayscale, output, CV_GRAY2BGR) : output = plateImageGrayscale.clone();

	return output;
}

void filter::algos::IDPlateRectifier::normalizeRatios(int leftBound, int rightBound)
{
	if (!isInInclusiveRange(leftRatio, leftBound, rightBound) || !isInInclusiveRange(rightRatio, leftBound, rightBound) || !isInInclusiveRange(topRatio, leftBound, rightBound))
	{
		throw HipeException("Error in IDPlateRectifier: Search ratios params are outside permitted range (0%-100%)");
	}

	if (rightBound < leftBound)
	{
		int temp = rightBound;
		rightBound = leftBound;
		leftBound = temp;
	}

	if (leftBound == 0 && rightBound == 100)
	{
		leftRatio /= 100;
		rightRatio /= 100;
		topRatio /= 100;
	}
	else
	{
		int range = rightBound - leftBound;
		leftRatio = (leftRatio - leftBound) / range;
		leftRatio = leftRatio * range + leftBound;
		rightRatio = (rightRatio - leftBound) / range;
		rightRatio = rightRatio * range + leftBound;
		topRatio = (topRatio - leftBound) / range;
		topRatio = topRatio * range + leftBound;
	}

}
bool filter::algos::IDPlateRectifier::isInInclusiveRange(double value, double leftBound, double rightBound)
{

	return (value >= leftBound && value <= rightBound);
}


