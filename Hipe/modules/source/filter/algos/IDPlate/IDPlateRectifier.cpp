#include <filter/algos/IDPlate/IDPlateRectifier.h>


HipeStatus filter::algos::IDPlateRectifier::process()
{
	while (!_connexData.empty())
	{
		data::ImageArrayData data = _connexData.pop();
		data::ImageArrayData outputData;

		for (auto& image : data.Array_const())
		{
			//// Debug: searchlines limits
			//normalizeRatios();

			// Find ID plate's characters
			const double drawContourThickness = CV_FILLED;
			cv::Mat binarizedImage;
			std::vector<cv::Rect> plateCharacters = filter::algos::IDPlate::findPlateCharacter(image, binarizedImage, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, cv::Size(charMinWidth, charMinHeight), drawContourThickness, _debug);

			// Separate them by rows (lines)
			std::vector<int> characterRows = filter::algos::IDPlate::splitImgByCharRows(image, plateCharacters);

			//// Add line to bottom of image (we want rows separated by top and bottom lines)
			//characterRows.push_back(image.rows - 1);

			//Debug
			if (_debug)
			{
				cv::Mat debugImage = image.clone();
				for (auto & row : characterRows)
				{
					cv::line(debugImage, cv::Point(0, row), cv::Point(debugImage.cols - 1, row), cv::Scalar(0, 0, 255), 4);
				}
				filter::algos::IDPlate::showImage(debugImage);
			}

			// Sort characters by rows
			std::vector<std::vector<cv::Rect>> charactersSorted = filter::algos::IDPlate::splitCharactersByRows(plateCharacters, characterRows, image, _debug);

			// Find whole text area
			std::vector<cv::Point> bounds = findCharactersBounds(image, charactersSorted);

			// Then extract it
			cv::Mat rectifiedPlate = perspectiveCrop(image, bounds);

			// Debug
			if (_debug)	filter::algos::IDPlate::showImage(rectifiedPlate);

			outputData << rectifiedPlate;
		}

		PUSH_DATA(outputData);
	}
	return OK;
}

bool filter::algos::IDPlateRectifier::compByDeriv(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.first > b.first;
}

bool filter::algos::IDPlateRectifier::compRectByHPos(const cv::Rect & a, const cv::Rect & b)
{
	return a.x < b.x;
}

std::vector<cv::Point> filter::algos::IDPlateRectifier::findCharactersBounds(const cv::Mat & image, const std::vector<std::vector<cv::Rect>>& charactersSorted)
{
	// Extract the 3 characters lines with the longest characters chain
	std::vector<std::vector<cv::Rect>> bestLines = findLongestTextLines(3, charactersSorted);

	// Sort the rects in the lines by position
	for (auto & line : bestLines)
	{
		std::sort(line.begin(), line.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compRectByHPos(a, b); });
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
		// Exclude lines too short
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

	int bestIndex = -1;		// TODO: Exception
	double bestSumDeriv2 = std::numeric_limits<double>::max();

	//Select best index by deriv second 
	for (int i = 0; i < nbDeriv.size() - linesToFind + 1; i++)
	{
		double deriv2Mean = 0.0;

		if (nbDeriv[i].first == 0 || nbDeriv[i].second == 0.0)
			continue;
		if (i + linesToFind > nbDeriv.size())	// TODO: Exception
			break;
		bool isEmpty = false;

		for (int j = 0; j < linesToFind; j++)
		{
			if (nbDeriv[i + j].first == 0 || nbDeriv[i + j].second == 0.0)
			{
				isEmpty = true;
				break;
			}


			//deriv2Mean += nbDeriv[i + j].second;
			deriv2Mean += nbDeriv[i + j].second / nbDeriv[i + j].first;
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
		longestLines[j] = textList[bestIndex + j];	// TODO: Exception
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

	// Find characters' englobing 4 corners (min/max left x/y coords min/max right x/y coords)
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


		if (debug && _debug)
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
			filter::algos::IDPlate::showImage(temp);
		}
	}

	cv::Point topLeft(xMinLeft, yMinLeft);
	cv::Point topRight(xMaxRight, yMinRight);

	if (debug && _debug)
	{
		cv::Mat temp = image.clone();
		cv::line(temp, topLeft, topRight, cv::Scalar(255, 255, 0), 4);
		filter::algos::IDPlate::showImage(temp);
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

	if (debug && _debug)
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

		filter::algos::IDPlate::showImage(temp);
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
	cv::Mat image = filter::algos::IDPlate::convertColor2Gray(plateImage);

	std::vector<cv::Point> newCorners(textCorners);

	// Preprocess image (binary & bigger lines)
	cv::Mat binaryImage;
	cv::Canny(image, binaryImage, 100, 50, 3, true);
	binaryImage = filter::algos::IDPlate::applyMorphTransform(binaryImage, cv::MorphShapes::MORPH_CROSS, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));

	// Extract the image's Horizontal lines
	cv::Mat horizontalLines;
	const int horizontalFactor = 30;
	const int horizontalSize = image.cols / horizontalFactor;

	cv::Mat horizontalKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(horizontalFactor, 1));
	cv::erode(binaryImage, horizontalLines, horizontalKernel);
	cv::dilate(horizontalLines, horizontalLines, horizontalKernel);
	cv::dilate(horizontalLines, horizontalLines, cv::Mat::ones(3, 3, CV_8UC1));
	// Debug
	if (_debug)filter::algos::IDPlate::showImage(horizontalLines);

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
	if (_debug)filter::algos::IDPlate::showImage(verticalLines);

	// Find left line
	std::vector<cv::Point> leftCorners = findAreaLeftLine(verticalLines, newCorners);
	newCorners[0] = leftCorners[0];
	newCorners[3] = leftCorners[1];

	// Find right line
	std::vector<cv::Point> rightCorners = findAreaRightLine(verticalLines, newCorners);
	newCorners[1] = rightCorners[0];
	newCorners[2] = rightCorners[1];


	// Debug
	if (_debug)
	{
		cv::Mat temp(plateImage.clone());
		cv::circle(temp, newCorners[2], 4, cv::Scalar(255, 255, 0), 1);
		cv::circle(temp, newCorners[3], 4, cv::Scalar(255, 255, 0), 1);
		filter::algos::IDPlate::showImage(temp);
	}

	// Clamp values
	for(int i = 0; i < newCorners.size(); ++i)
	{
		newCorners[i].x = std::max(0, std::min(newCorners[i].x, plateImage.cols - 1));
		newCorners[i].y = std::max(0, std::min(newCorners[i].y, plateImage.rows - 1));
	}

	// Debug
	if (_debug)
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

		filter::algos::IDPlate::showImage(newTextArea);
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
	//pt2.x = cv::max(0, (int)(pt2.x /*- abs(xmax - xmin) / 4.0*/));
	pt2.x = cv::max(0, static_cast<int>(pt2.x));

	// Debug
	if (_debug)
	{
		cv::Mat searchLine = filter::algos::IDPlate::convertGray2Color(plateHorizontalLines);
		cv::line(searchLine, cv::Point(pt1.x, 0), pt1, cv::Scalar(0, 0, 255), 1);
		cv::line(searchLine, cv::Point(pt2.x, 0), pt2, cv::Scalar(0, 0, 255), 1);
		cv::circle(searchLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(searchLine, pt2, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(searchLine);
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
		cv::Point2f oldPtNnext(ptNext);
		ptNext = _findNextTopPivotPoint(plateHorizontalLines, ptNext, limit);
		// TM: Avoid looping on same point
		if (oldPtNnext == ptNext)	ptNext = _findNextTopPivotPoint(plateHorizontalLines, cv::Point2f(ptNext.x, ptNext.y - 1), limit);
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

			if (_debug > 1)
			{
				cv::Mat topLine = filter::algos::IDPlate::convertGray2Color(plateHorizontalLines);
				cv::line(topLine, pt1, cartesianMax, cv::Scalar(255, 0, 0), 2);
				cv::circle(topLine, pt1, 2, cv::Scalar(255, 255, 0), 1);
				cv::circle(topLine, cartesianMax, 2, cv::Scalar(255, 255, 0), 1);
				filter::algos::IDPlate::showImage(topLine);
			}
		}
	}

	// Tentative FIX endpoint enside image bounds: Clamp cartesianMax
	cartesianMax.x = std::max(0, std::min(cartesianMax.x, plateHorizontalLines.cols - 1));
	cartesianMax.y = std::max(0, std::min(cartesianMax.y, plateHorizontalLines.rows - 1));

	// Debug
	if (_debug)
	{
		cv::Mat topLine = filter::algos::IDPlate::convertGray2Color(plateHorizontalLines);
		cv::line(topLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 1);
		cv::circle(topLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(topLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(topLine);
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
	if (_debug)
	{
		cv::Mat topLine = filter::algos::IDPlate::convertGray2Color(plateHorizontalLines);
		cv::line(topLine, areaTopLeftCorner, areaTopRightCorner, cv::Scalar(0, 0, 255), 1);
		cv::circle(topLine, areaTopLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(topLine, areaTopRightCorner, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(topLine);
	}

	return topLineCorners;
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
	pt1.y = cv::min(plateVerticalLines.rows - 1, static_cast<int>(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = cv::max(0, static_cast<int>(pt2.y - abs(ymax - ymin) / 6.0));				// 6 is the middle of 3 lines of text


	// Debug
	if (_debug)
	{
		cv::Mat searchLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
		cv::line(searchLine, cv::Point(0, pt1.y), pt1, cv::Scalar(0, 0, 255), 1);
		cv::line(searchLine, cv::Point(0, pt2.y), pt2, cv::Scalar(0, 0, 255), 1);
		cv::circle(searchLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(searchLine, pt2, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(searchLine);
	}
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
		cv::Point2f oldPtNext(ptNext);
		ptNext = _findNextLeftPivotPoint(plateVerticalLines, ptNext, limit);
		// TM: Avoid looping on same point
		if (oldPtNext == ptNext)	ptNext = _findNextLeftPivotPoint(plateVerticalLines, cv::Point2f(ptNext.x - 1, ptNext.y), limit);
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

			if (_debug > 1)
			{
				cv::Mat leftLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
				cv::line(leftLine, pt1, cartesianMax, cv::Scalar(255, 0, 0), 2);
				cv::circle(leftLine, pt1, 2, cv::Scalar(255, 255, 0), 1);
				cv::circle(leftLine, cartesianMax, 2, cv::Scalar(255, 255, 0), 1);
				filter::algos::IDPlate::showImage(leftLine);
			}
		}

	}

	// Tentative FIX endpoint inside image bounds: Clamp cartesianMax
	cartesianMax.x = std::max(0, std::min(cartesianMax.x, plateVerticalLines.cols - 1));
	cartesianMax.y = std::max(0, std::min(cartesianMax.y, plateVerticalLines.rows - 1));

	// Debug
	if (_debug)
	{
		cv::Mat leftLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
		cv::line(leftLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 2);
		cv::circle(leftLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(leftLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(leftLine);
	}
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
	if (_debug)
	{
		cv::Mat leftLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
		cv::line(leftLine, areaTopLeftCorner, areaBottomLeftCorner, cv::Scalar(0, 0, 255), 1);
		cv::circle(leftLine, areaTopLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(leftLine, areaBottomLeftCorner, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(leftLine);

		cv::Mat upperLeftCorner = leftLine;
		cv::line(upperLeftCorner, areaTopLeftCorner, textCorners[1], cv::Scalar(0, 0, 255), 1);
		cv::circle(upperLeftCorner, textCorners[1], 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(upperLeftCorner);
	}
	return LeftCorners;
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
	pt1.y = cv::min(plateVerticalLines.rows - 1, static_cast<int>(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = cv::max(0, static_cast<int>(pt2.y - abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
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
		cv::Point2f oldPtNext(ptNext);
		ptNext = _findNextRightPivotPoint(plateVerticalLines, ptNext, limit);
		// TM: Avoid looping on same point
		if (oldPtNext == ptNext) ptNext = _findNextRightPivotPoint(plateVerticalLines, cv::Point2f(ptNext.x + 1, ptNext.y), limit);
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

			if (_debug > 1)
			{
				cv::Mat rightLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
				cv::line(rightLine, pt1, cartesianMax, cv::Scalar(255, 0, 0), 2);
				cv::circle(rightLine, pt1, 2, cv::Scalar(255, 255, 0), 1);
				cv::circle(rightLine, cartesianMax, 2, cv::Scalar(255, 255, 0), 1);
				filter::algos::IDPlate::showImage(rightLine);
			}
		}
	}

	// Tentative FIX endpoint enside image bounds: Clamp cartesianMax
	cartesianMax.x = std::max(0, std::min(cartesianMax.x, plateVerticalLines.cols - 1));
	cartesianMax.y = std::max(0, std::min(cartesianMax.y, plateVerticalLines.rows - 1));

	// Debug
	if (_debug)
	{
		cv::Mat rightLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
		cv::line(rightLine, pt1, cartesianMax, cv::Scalar(0, 0, 255), 2);
		cv::circle(rightLine, pt1, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(rightLine, cartesianMax, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(rightLine);
	}
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
	if (_debug)
	{
		cv::Mat rightLine = filter::algos::IDPlate::convertGray2Color(plateVerticalLines);
		cv::line(rightLine, areaTopRightCorner, areaBottomRightCorner, cv::Scalar(0, 0, 255), 1);
		cv::circle(rightLine, areaTopRightCorner, 3, cv::Scalar(255, 255, 0), 3);
		cv::circle(rightLine, areaBottomRightCorner, 3, cv::Scalar(255, 255, 0), 3);
		filter::algos::IDPlate::showImage(rightLine);
	}
	return rightCorners;
}

cv::Point filter::algos::IDPlateRectifier::_findNextTopPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin, end;

	// Look in line for next white pixel
	cv::Mat col = plateImage.col(currPosition.x).t();
	const uchar* pCol = col.ptr<uchar>(0);

	for (int y = currPosition.y; y >= limit.y && y >= 0; y--)
	{
		const int value = static_cast<int>(pCol[y]);

		// TODO: set debug level
		if (_debug > 2)
		{
			cv::Mat dbg1 = filter::algos::IDPlate::convertGray2Color(plateImage);
			cv::Mat dbg2 = filter::algos::IDPlate::convertGray2Color(col);

			cv::circle(dbg1, cv::Point(currPosition.x, y), 2, cv::Scalar(0, 0, 255));
			cv::circle(dbg2, cv::Point(currPosition.x, y), 1, cv::Scalar(0, 0, 255));

			filter::algos::IDPlate::showImage(dbg1);
			filter::algos::IDPlate::showImage(dbg2);
		}

		// First white pixel found case
		if (!foundBegin && value == 255)
		{
			foundBegin = true;
			begin = y;
			continue;
		}

		// Second white pixel found case
		if (foundBegin && value == 0)
		{
			end = cv::max(0, y + 1);

			// Output middle of found white line
			cv::Point output(currPosition.x, cv::max(0, begin + (end - begin) / 2));

			return output;
		}
	}
	// No white pixel found
	if (!foundBegin) return cv::Point(-1, -1);

	return cv::Point(currPosition.x, begin / 2);
}

cv::Point filter::algos::IDPlateRectifier::_findNextLeftPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin, end;

	cv::Mat row = plateImage.row(currPosition.y);
	const uchar* pRow = row.ptr<uchar>(0);

	//Now look for next line by white pixel
	for (int x = currPosition.x; x > limit.x && x >= 0; x--)
	{
		const int value = static_cast<int>(pRow[x]);

		// Debug
		if (_debug > 2)
		{
			cv::Mat dbg1 = filter::algos::IDPlate::convertGray2Color(plateImage);
			cv::Mat dbg2 = filter::algos::IDPlate::convertGray2Color(row);

			cv::circle(dbg1, cv::Point(x, currPosition.y), 2, cv::Scalar(0, 0, 255));
			cv::circle(dbg2, cv::Point(x, currPosition.y), 1, cv::Scalar(0, 0, 255));

			filter::algos::IDPlate::showImage(dbg1);
			filter::algos::IDPlate::showImage(dbg2);
		}

		if (!foundBegin && value == 255)
		{
			foundBegin = true;
			begin = x;
			continue;
		}

		if (foundBegin && value == 0)
		{
			end = x + 1;
			cv::Point output(cv::max(0, end + (begin - end) / 2), currPosition.y);

			return output;
		}
	}

	if (!foundBegin) return cv::Point(-1, -1);

	cv::Point output(begin / 2, currPosition.y);
	return output;
}

cv::Point filter::algos::IDPlateRectifier::_findNextRightPivotPoint(const cv::Mat & plateImage, const cv::Point & currPosition, const cv::Point & limit)
{
	bool foundBegin = false;
	int begin, end;

	cv::Mat row = plateImage.row(currPosition.y);
	const uchar* pRow = row.ptr<uchar>(0);

	//Now look for next line by white pixel
	for (int x = currPosition.x; x < limit.x && x < plateImage.cols; x++)
	{
		int value = static_cast<int>(pRow[x]);

		// Debug
		if (_debug > 2)
		{
			cv::Mat dbg1 = filter::algos::IDPlate::convertGray2Color(plateImage);
			cv::Mat dbg2 = filter::algos::IDPlate::convertGray2Color(row);

			cv::circle(dbg1, cv::Point(x, currPosition.y), 2, cv::Scalar(0, 0, 255));
			cv::circle(dbg2, cv::Point(x, currPosition.y), 1, cv::Scalar(0, 0, 255));

			filter::algos::IDPlate::showImage(dbg1);
			filter::algos::IDPlate::showImage(dbg2);
		}


		if (!foundBegin && value == 255)
		{
			foundBegin = true;
			begin = x;
			continue;
		}

		if (foundBegin && value == 0)
		{
			end = x - 1;
			cv::Point output(cv::max(0, begin + (end - begin) / 2), currPosition.y);

			return output;
		}
	}

	if (!foundBegin) return cv::Point(-1, -1);

	cv::Point output(begin + (plateImage.cols - 1 - begin) / 2, currPosition.y);
	return output;
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

	double dbgValX, dbgValY;
	cv::Point2f dbgBestPoint;

	// Rotate around pivot point from max angle to min angle
	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sinT = sin(t);
		double cosT = cos(t);

		// Analyze each pixel value from pivot point and rotating line. The one with the largest sum of white pixel is the line we're looking for
		for (double r = 1; r < rho; r += strideRho)
		{
			int x = origin.x + r * cosT;
			int y = origin.y + r * sinT;

			// Clamping values to image bounds induce errors (pixel on bound value is counted multiple times)
			if (!(x >= 0 && x < image.cols) || !(y >= 0 && y < image.rows)) break;

			if (_debug > 2)
			{
				cv::Mat temp = filter::algos::IDPlate::convertGray2Color(image);
				cv::circle(temp, origin, 4, cv::Scalar(0, 255, 0), 2);
				cv::circle(temp, cv::Point(x, y), 2, cv::Scalar(0, 0, 255));
				filter::algos::IDPlate::showImage(temp);
			}

			const uchar* pRow = image.ptr<uchar>(y);
			const int value = static_cast<int>(pRow[x]);
			if (value == 255)
			{
				sum++;
			}
		}

		// Debug
		if (_debug > 2)
		{
			dbgValX = origin.x + rho * cosT;
			dbgValY = origin.y + rho * sinT;
		}

		if (_debug > 2)
		{
			cv::Mat searchLine = filter::algos::IDPlate::convertGray2Color(image);
			cv::circle(searchLine, origin, 3, cv::Scalar(0, 0, 255), 3);
			cv::circle(searchLine, cv::Point2f(dbgValX, dbgValY), 3, cv::Scalar(0, 0, 255), 3);
			cv::line(searchLine, origin, cv::Point2f(dbgValX, dbgValY), cv::Scalar(0, 255, 0), 2);
			filter::algos::IDPlate::showImage(searchLine);
		}
		// Each time we find a new best line, store the angle
		if (sum >= sumMax)
		{
			sumMax = sum;
			tBest = t;

			// Debug
			if (_debug > 2)	dbgBestPoint = cv::Point2f(dbgValX, dbgValY);
		}
	}

	if (_debug > 2)
	{
		// Debug
		cv::Mat temp = filter::algos::IDPlate::convertGray2Color(image);
		cv::circle(temp, origin, 2, cv::Scalar(255, 0, 0), 3);
		cv::circle(temp, dbgBestPoint, 2, cv::Scalar(255, 0, 0), 3);
		cv::line(temp, origin, dbgBestPoint, cv::Scalar(0, 0, 255), 4);
		filter::algos::IDPlate::showImage(temp);
	}
	// Output angle
	out_bestLineParameters = cv::Vec2f(rho, tBest);

	return sumMax;
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
	int sumMax = 0;
	double tBest = 0.0;
	double dbgValX = 0;
	double dbgValY = 0;
	cv::Point2f dbgBestPoint;

	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sinT = sin(t);
		double cosT = cos(t);

		for (double r = 0.0; r < rho; r += strideRho)
		{
			int x = origin.x + r * cosT + 0.5;
			int y = origin.y + r * sinT + 0.5;

			// Clamping values to image bounds induce errors (pixel on bound value is counted multiple times)
			if (!(x >= 0 && x < image.cols) || !(y >= 0 && y < image.rows)) break;

			//int x = cv::min(image.cols - 1, static_cast<int>(origin.x + r * cosT + 0.5));
			//x = cv::max(0, x);

			//int y = cv::min(image.rows - 1, static_cast<int>(origin.y + r * sinT + 0.5));
			//y = cv::max(0, y);

			//if (_debug && (x <= 10 || x >= 800))
			if (_debug > 2)
			{
				cv::Mat temp = filter::algos::IDPlate::convertGray2Color(image);
				cv::circle(temp, origin, 4, cv::Scalar(0, 255, 0), 2);
				cv::circle(temp, cv::Point(x, y), 2, cv::Scalar(0, 0, 255));
				filter::algos::IDPlate::showImage(temp);
			}

			// Read pixel value
			const uchar *pRow = image.ptr<uchar>(y);
			const int value = static_cast<int>(pRow[x]);

			if (value == 255)
			{
				sum++;
			}
		}

		// Debug


		if (_debug > 2)
		{
			dbgValX = origin.x + rho * cosT + 0.5;
			dbgValY = origin.y + rho * sinT + 0.5;

			cv::Mat searchLine = filter::algos::IDPlate::convertGray2Color(image);
			cv::circle(searchLine, origin, 3, cv::Scalar(0, 0, 255), 3);
			cv::circle(searchLine, cv::Point2f(dbgValX, dbgValY), 3, cv::Scalar(0, 0, 255), 3);
			cv::line(searchLine, origin, cv::Point2f(dbgValX, dbgValY), cv::Scalar(0, 255, 0), 2);
			filter::algos::IDPlate::showImage(searchLine);
		}

		if (sum >= sumMax)
		{
			sumMax = sum;
			tBest = t;

			// Debug
			if (_debug > 2)	dbgBestPoint = cv::Point2f(dbgValX, dbgValY);
		}
	}

	// Debug
	if (_debug > 2)
	{
		cv::Mat temp = filter::algos::IDPlate::convertGray2Color(image);
		cv::circle(temp, origin, 2, cv::Scalar(255, 0, 0), 3);
		cv::circle(temp, dbgBestPoint, 2, cv::Scalar(255, 0, 0), 3);
		cv::line(temp, origin, dbgBestPoint, cv::Scalar(0, 0, 255), 4);
		filter::algos::IDPlate::showImage(temp);
	}

	out_bestLineParameters = cv::Vec2f(rho, tBest);

	return sumMax;
}



cv::Point2f filter::algos::IDPlateRectifier::computeLinesIntersectionPoint(const cv::Vec4i & a, const cv::Vec4i & b)
{
	int x1 = a[0], y1 = a[1];
	int x2 = a[2], y2 = a[3];
	int x3 = b[0], y3 = b[1];
	int x4 = b[2], y4 = b[3];

	// If denom equals 0, lines are parallel
	const int denom = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4));
	if (denom != 0)
	{
		cv::Point2f pt;
		pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / static_cast<double>(denom);
		pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / static_cast<double>(denom);
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

cv::Mat filter::algos::IDPlateRectifier::perspectiveCrop(const cv::Mat & plateImage, const std::vector<cv::Point>& textCorners)
{
	if (!plateImage.data)
	{
		throw HipeException("[ERROR] IDPlateRectifier::perspectiveCrop - Invalid image");
	}

	//Debug
	if (_debug)
	{
		cv::Mat temp = plateImage.clone();
		cv::circle(temp, textCorners[0], 3, cv::Scalar(0, 0, 255), 2);
		cv::circle(temp, textCorners[1], 3, cv::Scalar(0, 0, 255), 2);
		cv::circle(temp, textCorners[2], 3, cv::Scalar(0, 0, 255), 2);
		cv::circle(temp, textCorners[3], 3, cv::Scalar(0, 0, 255), 2);

		cv::line(temp, textCorners[0], textCorners[1], cv::Scalar(0, 0, 255));
		cv::line(temp, textCorners[1], textCorners[2], cv::Scalar(0, 0, 255));
		cv::line(temp, textCorners[2], textCorners[3], cv::Scalar(0, 0, 255));
		cv::line(temp, textCorners[3], textCorners[0], cv::Scalar(0, 0, 255));
		filter::algos::IDPlate::showImage(temp);
	}
	// From the 4 corners find the homography matrix to output a flat image
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

	cv::Mat output = cv::Mat::zeros(boundingRect.height, boundingRect.width, CV_8UC3);

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(perspectiveInput, perspectiveOutput);
	cv::warpPerspective(plateImage, output, perspectiveTransform, output.size());

	return output;
}
