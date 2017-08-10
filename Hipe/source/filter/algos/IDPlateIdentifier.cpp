#include <filter/Algos/IDPlateIdentifier.h>

HipeStatus filter::Algos::IDPlateIdentifier::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	// Find characters
	LabelOCR labelOCR(true);
	std::vector<cv::Rect> charactersRects;

	findCharacters(image);
	cv::Mat output = image;
	//std::vector<cv::Mat> characters = detectTextArea(image, charactersRects);

	//// Identify them using Tesseract
	//std::vector<std::string> labels = labelOCR.runRecognition(characters, 30);	// legacy: labelType = 2

	//// Output results in an image
	//cv::Mat output = createOutputImage(image, charactersRects, labels);

	_connexData.push(output);
	return OK;
}

std::vector<cv::Mat> filter::Algos::IDPlateIdentifier::detectTextArea(const cv::Mat & plateImage, std::vector<cv::Rect> & out_charactersRects)
{
	cv::Mat image = plateImage;

	// Preprocess image
	cv::Mat preprocessed = preprocessImage(plateImage);
	cv::Mat binarizedImage;

	// Find characters
	//std::vector<cv::Rect> characters = filter::algos::IDPlate::findPlateCharacters(preprocessed, 0.04f, .8f, _debug, 4, binarizedImage); // Old values were minPosX 0.1, maxPosX 1.0, ratioLowerBound 0.20, ratioUpperBound 0.80

	const double drawContourThickness = 4;
	std::vector<cv::Rect> characters = filter::algos::IDPlate::findPlateCharacter(preprocessed, binarizedImage, charMinXBound, charMaxXBound, charMinFillRatio, charMaxFillRatio, cv::Size(charMinWidth, charMinHeight), drawContourThickness, _debug);

	if (_debug)
	{
		cv::Mat charactersBounds = image.clone();
		for (auto & character : characters)
		{
			cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
		}
		filter::algos::IDPlate::showImage(charactersBounds);
	}

	// Sort and filter them (how ?)
	std::vector<cv::Rect> charactersFiltered = sortAndFilterCharacters(characters, plateImage.rows, 0.06, 0.35);

	if (_debug)
	{
		cv::Mat charactersBounds = image.clone();
		for (auto & character : charactersFiltered)
		{
			cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
		}
		filter::algos::IDPlate::showImage(charactersBounds);
	}

	//// ?
	//charactersFiltered = blobsRectangle(preprocessed, charactersFiltered);

	//// Separate characters by lines
	//std::vector<std::vector<cv::Rect>> debugChars = separateCharactersByLines(charactersFiltered, plateImage);

	// Extract characters: crop image with rectangles
	std::vector<cv::Mat> croppedCharacters;
	for (auto & character : charactersFiltered)
	{
		cv::Mat clone(image(character).clone());
		croppedCharacters.push_back(clone);


		//if(_debug)	filter::algos::IDPlate::showImage(clone);
	}

	// Return characters rects
	out_charactersRects = charactersFiltered;
	// Return cropped images
	return croppedCharacters;
}

cv::Mat filter::Algos::IDPlateIdentifier::preprocessImage(const cv::Mat & plateImage)
{
	cv::Mat image = plateImage.clone();

	//// Downsample image
	//cv::pyrDown(image, image);

	// Smooth image to reduce noise
	int diameter = 15;
	double sigmaColor = 15.0 * 2.0;
	double sigmaSpace = 15.0 * 0.5;

	image = filter::algos::IDPlate::applyBilateralFiltering(image, 1, diameter, sigmaColor, sigmaSpace, _debug, false);

	// Convert to grayscale
	image = filter::algos::IDPlate::convertColor2Gray(image);

	// Improve contrast
	// Create a CLAHE object(Arguments are optional).
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0);
	clahe->apply(image, image);
	if (_debug)	filter::algos::IDPlate::showImage(image);

	// Binarize image
	//cv::GaussianBlur(image, image, cv::Size(3, 3), 0);
	cv::bitwise_not(image, image);
	if (_debug)	filter::algos::IDPlate::showImage(image);

	return image;
}

std::vector<cv::Rect> filter::Algos::IDPlateIdentifier::sortAndFilterCharacters(std::vector<cv::Rect>& characters, int plateImageRows, double lowerBound, double upperBound)
{
	std::vector<cv::Rect> charactersFiltered;

	// Sort characters
	//std::sort(characters.begin(), characters.end(), [](const cv::Rect& a, const cv::Rect& b) { filter::algos::IDPlate::compRectsByPos(a, b); });
	std::sort(characters.begin(), characters.end(), filter::algos::IDPlate::CompRectsByPos());

	if (lowerBound > upperBound)
	{
		double temp = upperBound;
		upperBound = lowerBound;
		lowerBound = temp;
	}

	// Filter out
	for (auto & character : characters)
	{
		if (static_cast<double>(character.height) / static_cast<double>(plateImageRows) > upperBound) continue;
		if (static_cast<double>(character.height) / static_cast<double>(plateImageRows) < lowerBound) continue; // area is a line ?? or something else ?

		//// Dilate width the right of the rectangle to check if there is more rect 
		//textAreas[i].width += 0.50 * textAreas[i].width;

		charactersFiltered.push_back(character);
	}

	return charactersFiltered;
}

//std::vector<cv::Rect> filter::Algos::IDPlateIdentifier::blobsRectangle(const cv::Mat & plateImage, std::vector<cv::Rect>& characters)
//{
//	std::vector<cv::Rect> filteredTextAreas;
//
//	// Tri des rects (ici déjà triés)
//	//std::sort(characters.begin(), characters.end(), [](const cv::Rect& a, const cv::Rect& b) { filter::algos::IDPlate::compRectsByPos(a, b); });
//	std::sort(characters.begin(), characters.end(), filter::algos::IDPlate::CompRectsByPos());
//
//	//// Concatenation caractères + caractères filtrés ? Manque quelque chose ?
//	//for (int i = 0; i < filteredTextAreas.size(); i++)
//	//	characters.push_back(filteredTextAreas[i]);
//	//filteredTextAreas.clear();
//
//	const int deltaY = 4;
//	const double deltaXRatio = 0.60;
//
//	// Pour chaque caractères de la liste
//	for (int i = 0, j = 0; i < characters.size() && j < characters.size(); i++)
//	{
//		// Ajouter caractère à liste
//		filteredTextAreas.push_back(characters[i]);
//
//		// référence du rect en fin de liste
//		cv::Rect& lastRect = filteredTextAreas.back();
//
//		// Point au centre de sa ligne de droite
//		cv::Point lastRectCenter = cv::Point(lastRect.x + lastRect.width, lastRect.y + lastRect.height / 2);
//
//
//		// Debug
//		cv::Mat temp;
//		if (_debug)
//		{
//			temp = filter::algos::IDPlate::convertGray2Color(plateImage);
//			// Current character (i)
//			cv::rectangle(temp, characters[i], cv::Scalar(255, 0, 0), 1);
//			// Last character (size-1)
//			cv::rectangle(temp, lastRect, cv::Scalar(255, 0, 0), 1);
//			// Last character right line center
//			cv::circle(temp, lastRectCenter, 2, cv::Scalar(0, 0, 255), 2);
//			filter::algos::IDPlate::showImage(temp);
//		}
//
//		for (j = i + 1; j < characters.size(); j++)
//		{
//			// Caractère suivant
//			cv::Rect& nextRect = characters[j];
//
//			// Point au centre de sa ligne de gauche
//			cv::Point nextRectCenter = cv::Point(nextRect.x, nextRect.y + nextRect.height / 2);
//
//			// Debug
//			if (_debug)
//			{
//				// Next Character (i + 1)
//				cv::rectangle(temp, characters[j], cv::Scalar(255, 255, 0), 1);
//				// Next Character left line center
//				cv::circle(temp, nextRectCenter, 4, cv::Scalar(255, 0, 255), 2);
//				filter::algos::IDPlate::showImage(temp);
//			}
//			// Consider that 4 pixels or under is the same line of character
//			const double deltaX = deltaXRatio * characters[j].width;
//
//			if (abs(nextRectCenter.y - lastRectCenter.y) <= deltaY &&
//				nextRectCenter.x > lastRect.x && nextRectCenter.x < lastRect.x + lastRect.width + deltaX)
//			{
//				// Debug
//				if(_debug)
//				{
//					cv::Mat temp2 = filter::algos::IDPlate::convertGray2Color(plateImage);
//
//					cv::Rect tempRect(lastRect);
//					tempRect.x = (cv::min)(characters[j].x, tempRect.x);
//					tempRect.y = (cv::min)(characters[j].y, tempRect.y);
//
//					tempRect.width = characters[j].x + characters[j].width - tempRect.x;
//					tempRect.height = (cv::max)(characters[j].height, tempRect.height);
//
//					// Draw current rect
//					cv::rectangle(temp2, characters[i], cv::Scalar(255, 0, 0), 1);
//					// Draw last rect
//					cv::rectangle(temp2, lastRect, cv::Scalar(255, 0, 0), 1);
//					// Draw tempRect
//					cv::rectangle(temp2, tempRect, cv::Scalar(0, 255, 0), 1);
//					filter::algos::IDPlate::showImage(temp2);
//				}
//				continue;
//			}
//			else
//			{
//				//cv::rectangle(draw, characters[j], cv::Scalar(0, 0, 255), 1);
//				i = j - 1;
//				break;
//			}
//		}
//	}
//
//
//
//	// Debug
//	if (_debug)
//	{
//		cv::Mat temp = filter::algos::IDPlate::convertGray2Color(plateImage);
//		for (auto & character : filteredTextAreas)
//		{
//			cv::rectangle(temp, character, cv::Scalar(255, 0, 0), 1);
//		}
//		filter::algos::IDPlate::showImage(temp);
//	}
//
//	cv::Mat draw = filter::algos::IDPlate::convertGray2Color(plateImage);
//	const int inc = 10;
//	for (int i = 0; i < filteredTextAreas.size(); i++)
//	{
//		int oldX = filteredTextAreas[i].x;
//		int oldY = filteredTextAreas[i].y;
//		int oldWidth = filteredTextAreas[i].width;
//		int oldHeight = filteredTextAreas[i].height;
//
//		filteredTextAreas[i].x = (cv::max)(filteredTextAreas[i].x - inc / 2, 0);
//		filteredTextAreas[i].y = (cv::max)(filteredTextAreas[i].y - inc / 2, 0);
//
//		filteredTextAreas[i].width = filteredTextAreas[i].width + inc;
//		filteredTextAreas[i].height = filteredTextAreas[i].height + inc;
//
//		//// OLD
//		//filteredTextAreas[i].width = (filteredTextAreas[i].x + filteredTextAreas[i].width) > draw.cols ? filteredTextAreas[i].width - inc : filteredTextAreas[i].width;
//		//filteredTextAreas[i].height = (filteredTextAreas[i].y + filteredTextAreas[i].height) > draw.rows ? filteredTextAreas[i].height - inc : filteredTextAreas[i].height;
//
//		// NEW
//		int endWidth = filteredTextAreas[i].x + filteredTextAreas[i].width;
//		int endHeight = filteredTextAreas[i].y + filteredTextAreas[i].height;
//		int newWidth = draw.cols - 1 - filteredTextAreas[i].x;
//		int newHeight = draw.rows - 1 - filteredTextAreas[i].y;
//
//		filteredTextAreas[i].width = endWidth > draw.cols - 1 ? newWidth : filteredTextAreas[i].width;
//		filteredTextAreas[i].height = endHeight > draw.rows - 1 ? newHeight : filteredTextAreas[i].height;
//
//		// Debug
//		if(_debug)	cv::rectangle(draw, filteredTextAreas[i], cv::Scalar(255, 0, 0), 4);
//	}
//	// Debug
//	if(_debug)	filter::algos::IDPlate::showImage(draw);
//
//	return filteredTextAreas;
//}

std::vector<std::vector<cv::Rect>> filter::Algos::IDPlateIdentifier::separateCharactersByLines(std::vector<cv::Rect>& charactersSorted, const cv::Mat & debugImage)
{
	cv::Mat image = debugImage.clone();

	const int verticalDelta = 5;

	std::vector<std::vector<cv::Rect>> output;

	std::vector<cv::Rect>::const_iterator it = charactersSorted.begin();
	std::vector<cv::Rect>::const_iterator itNext = it + 1;
	std::vector<cv::Rect> line;

	while (it != charactersSorted.end() && itNext != charactersSorted.end())
	{
		// Add current point to line
		line.push_back(*it);

		// We Work on borders
		// Use Right border for first rect and left border for second one
		const cv::Point itBorder(it->x + it->width, it->y + it->height / 2);
		const cv::Point itNextBorder(itNext->x, itNext->y + itNext->height / 2);

		//Debug
		cv::Mat image = debugImage.clone();
		if (image.data && _debug > 2)
		{
			cv::rectangle(image, *it, cv::Scalar(255, 0, 0));
			cv::circle(image, itBorder, 2, cv::Scalar(0, 0, 0), 2);
			cv::rectangle(image, *itNext, cv::Scalar(255, 0, 0));
			cv::circle(image, itNextBorder, 2, cv::Scalar(255, 255, 255), 2);
			filter::algos::IDPlate::showImage(image);
		}

		// Compare borders y
		int delta = abs(itBorder.y - itNextBorder.y);
		// if outside delta, create new line
		if (delta > verticalDelta)
		{
			//Debug
			if (image.data && _debug > 2)
			{
				cv::rectangle(image, *itNext, cv::Scalar(0, 0, 255));
				filter::algos::IDPlate::showImage(image);
			}

			output.push_back(line);
			line.clear();
		}
		else
		{
			// Debug
			if (image.data && _debug > 2)
			{
				cv::rectangle(image, *itNext, cv::Scalar(0, 255, 0));
				filter::algos::IDPlate::showImage(image);
			}
		}

		it = itNext;
		++itNext;
	}

	// Push last point and last line
	line.push_back(*it);
	output.push_back(line);

	// Debug
	image = debugImage.clone();
	if (image.data && _debug > 2)
	{
		for (auto & line : output)
		{
			for (auto & rect : line)
			{
				cv::rectangle(image, rect, cv::Scalar(255, 0, 0), 2);
			}
		}

		filter::algos::IDPlate::showImage(image);
	}

	return output;
}

void filter::Algos::IDPlateIdentifier::findCharacters(const cv::Mat & plateImage)
{
	//// Preprocess image
	cv::Mat preprocessed = preprocessImage(plateImage);
	cv::Mat imageGrayscale = filter::algos::IDPlate::convertColor2Gray(preprocessed);
	cv::Mat debugImage = filter::algos::IDPlate::convertGray2Color(imageGrayscale);

	// First morphological transform
	imageGrayscale = filter::algos::IDPlate::applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));

	// Debug (show first morphology preprocess)
	if (_debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Debug (show converted (binary) image)
	if (_debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Second morphological transform
	imageGrayscale = filter::algos::IDPlate::applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));

	////////// DONT FORGET TO OUTPUT IMAGE WHEN CODE WILL BE SEPARATED IN DIFFERENT FUNCTIONS
	//////// Output "binarized" plate image
	//////out_binarizedImage = imageGrayscale.clone();

	// Debug (show second morphology transform)
	if (_debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat contoursDebug = imageGrayscale.clone();
	cv::findContours(contoursDebug, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	// Debug (show found contours)
	if (_debug)	filter::algos::IDPlate::showImage(contoursDebug);

	// Debug
	cv::Mat temp;
	if (_debug)	temp = plateImage.clone();

	//// Extract rects from found contours
	std::vector<cv::Rect> charactersFromContours;
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect currRect = cv::boundingRect(contours[idx]);

		//// fiter out really small rects
		//if (currRect.width <= minWidth && currRect.height <= minHeight)	continue;
		//if (currRect.width > maxWidth && currRect.height > maxHeight)	continue;

		charactersFromContours.push_back(currRect);

		// Debug (show current found contours' englobing rect)
		if (_debug)
		{
			cv::rectangle(temp, charactersFromContours.back(), cv::Scalar(255, 0, 0), 2);
			if (_debug > 2)
			{
				filter::algos::IDPlate::showImage(temp);
			}
		}
	}

	// Debug (show all found contours's englobing rects)
	if (_debug)	filter::algos::IDPlate::showImage(temp);


	//// Filter characters (1st pass)
	// From position (X axis)
	const int minXBound = dbgMinX * plateImage.cols;
	const int maxXBound = dbgMaxX * plateImage.cols;
	// From size [X + (X - 1) ; X]
	const int minLines = dbgMinLines;
	const int maxLines = 2 * minLines - 1;
	const int maxSize = plateImage.rows / minLines;
	const int minSize = plateImage.rows / maxLines;

	// Debug (show position bounds)
	if (_debug > 1)
	{
		temp = plateImage.clone();
		cv::line(temp, cv::Point(minXBound, 0), cv::Point(minXBound, temp.rows - 1), cv::Scalar(255, 0, 255), 2);
		cv::line(temp, cv::Point(maxXBound, 0), cv::Point(maxXBound, temp.rows - 1), cv::Scalar(255, 0, 255), 2);

		filter::algos::IDPlate::showImage(temp);
	}


	std::vector<cv::Rect> filteredCharacters, dubiousCharacters;
	for (auto & character : charactersFromContours)
	{
		// Debug (show processed rect in blue)
		if (_debug > 2)
		{
			cv::rectangle(temp, character, cv::Scalar(255, 0, 0), 1);
			cv::circle(temp, cv::Point(character.x, character.y), 2, cv::Scalar(0, 0, 255), 2);

			filter::algos::IDPlate::showImage(temp);
		}

		// Filter rect
		// TODO: Should filtering be inclusive or exclusive ?
		const bool position = character.x >= minXBound && character.x < maxXBound ? true : false;
		const bool size = character.height >= minSize && character.height < maxSize ? true : false;

		// Assert rect is within position (image X axis) bounds
		if (position)
		{
			// If size is correct accept char
			if (size)
			{
				filteredCharacters.push_back(character);

				// Debug (paint validated rect in green)
				if (_debug > 2)
				{
					cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 1);
				}
			}
			// If not, it in fact can possibly be a good one. Put it aside for a second pass validation test
			// Either way we do not want the rects outside the position bounds to be checked again.
			else
			{
				dubiousCharacters.push_back(character);

				// Debug (paint doubt on rect in yellow)
				if (_debug > 2)
				{
					cv::rectangle(temp, character, cv::Scalar(255, 255, 0), 1);
				}
			}
		}
		else
		{
			// Debug (paint rejected rect in red)
			if (_debug > 2)
			{
				cv::rectangle(temp, character, cv::Scalar(0, 0, 255), 1);
			}
		}

		// Debug (show result of rect analysis)
		if (_debug > 2) filter::algos::IDPlate::showImage(temp);
	}

	// Debug (show all validated and dubious rects)
	if (_debug)
	{
		temp = plateImage.clone();
		for (auto & character : filteredCharacters)
		{
			cv::rectangle(temp, character, cv::Scalar(0, 255, 0), 1);
		}
		for (auto & character : dubiousCharacters)
		{
			cv::rectangle(temp, character, cv::Scalar(255, 255, 0), 1);
		}

		filter::algos::IDPlate::showImage(temp);
	}

	//// Find characters lines and sort them
	// Sort rects by coordinates
	std::sort(filteredCharacters.begin(), filteredCharacters.end(), filter::algos::IDPlate::CompRectsByPos());
	std::sort(dubiousCharacters.begin(), dubiousCharacters.end(), filter::algos::IDPlate::CompRectsByPos());

	// Find lines
	std::vector<int> rowsPositions = filter::algos::IDPlate::splitImgByCharRows(plateImage, filteredCharacters);

	// Debug (show computed separator lines)
	if (_debug)
	{
		debugImage = plateImage.clone();
		for (auto & row : rowsPositions)
		{
			cv::line(debugImage, cv::Point(0, row), cv::Point(debugImage.cols - 1, row), cv::Scalar(0, 0, 255), 4);
		}

		filter::algos::IDPlate::showImage(debugImage);
	}

	// Split characters in lines
	// TODO: (las param must be _debug, not 0)
	std::vector<std::vector<cv::Rect>> charactersLines = filter::algos::IDPlate::splitCharactersByRows(filteredCharacters, rowsPositions, plateImage, 0);
	//std::vector<std::vector<cv::Rect>> dubiousCharactersLines = filter::algos::IDPlate::splitCharactersByRows(dubiousCharacters, rowsPositions, plateImage, 0);

	//// Refilter characters to validate dubious ones
	// Find each line median rect center value
	std::vector<filter::algos::IDPlate::LineData> charactersLinesData;
	for (auto & line : charactersLines)
	{
		filter::algos::IDPlate::LineData lineData;
		lineData.minWidth = lineData.minHeight = (std::numeric_limits<int>::max)();
		lineData.maxWidth = lineData.maxHeight = (std::numeric_limits<int>::min)();

		for (auto & character : line)
		{
			lineData.averageY += character.y + character.height / 2.0;
			lineData.averageCharWidth += character.width;
			lineData.averageCharHeight += character.height;

			if (character.width < lineData.minWidth)	lineData.minWidth = character.width;
			if (character.width > lineData.maxWidth)	lineData.maxWidth = character.width;
			if (character.height < lineData.minHeight)	lineData.minHeight = character.height;
			if (character.height > lineData.maxHeight)	lineData.maxHeight = character.height;
		}

		lineData.averageY /= line.size();
		lineData.averageCharWidth /= line.size();
		lineData.averageCharHeight /= line.size();

		charactersLinesData.push_back(lineData);
	}

	// Debug (show computed average Y values)
	if (_debug)
	{
		debugImage = plateImage.clone();
		for (auto & lineData : charactersLinesData)
		{
			cv::line(debugImage, cv::Point(0, lineData.averageY), cv::Point(plateImage.cols - 1, lineData.averageY), cv::Scalar(0, 0, 255), 2);
		}

		filter::algos::IDPlate::showImage(debugImage);
	}

	// Refilter characters using median value (the ones between 1st and last character on line)
	std::vector<cv::Rect> validCharacters;
	//const double heightRatio = 0.1;	// Ratio used (percentage) to compute delta in pixels
	for(int lineIdx = 0; lineIdx < charactersLines.size(); ++lineIdx)
	{
		// Assert characters will be between 1st and last rect
		const int minXPos = charactersLines[lineIdx].front().x;
		const int maxXPos = charactersLines[lineIdx].back().x + charactersLines[lineIdx].back().width;

		const filter::algos::IDPlate::LineData& lineData = charactersLinesData[lineIdx];

		// Delta in pixels used to validateCharacters
		const double deltaY = ratioY * lineData.averageCharHeight / 2.0;
		const double deltaHeight = ratioHeight * lineData.averageCharHeight / 2.0;
		const double deltaWidth = ratioWidth * lineData.averageCharWidth / 2.0;

		// Analyze every dubious character
		for(std::vector<cv::Rect>::iterator itDubiousChar = dubiousCharacters.begin(); itDubiousChar != dubiousCharacters.end();)
		{
			// Debug (show min X and max X computed from first and last character of line and show current dubious rect)
			if(_debug > 2)
			{
				debugImage = plateImage.clone();
				// X Bounds
				cv::line(debugImage, cv::Point(minXPos, 0), cv::Point(minXPos, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);
				cv::line(debugImage, cv::Point(maxXPos, 0), cv::Point(maxXPos, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);
				
				// Average Y
				cv::line(debugImage, cv::Point(0, lineData.averageY), cv::Point(debugImage.cols - 1, lineData.averageY), cv::Scalar(127, 0, 127), 2);
				cv::line(debugImage, cv::Point(0, lineData.averageY - deltaY), cv::Point(debugImage.cols - 1, lineData.averageY - deltaY), cv::Scalar(255, 0, 255), 1);
				cv::line(debugImage, cv::Point(0, lineData.averageY + deltaY), cv::Point(debugImage.cols - 1, lineData.averageY + deltaY), cv::Scalar(255, 0, 255), 1);
				
				// Dubious rect
				cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(255, 255, 0), 2);
				// Dubious rect Y
				cv::circle(debugImage, cv::Point(itDubiousChar->x, itDubiousChar->y + itDubiousChar->height / 2.0), 2, cv::Scalar(255, 255, 0), 2);

				filter::algos::IDPlate::showImage(debugImage);
			}
			// Character must be between first and last character of line
			// TODO: must bounds be inclusive or exclusive?
			if (itDubiousChar->x >= minXPos && itDubiousChar->x <= maxXPos)
			{
				// Character must be near median Y line and similar to others
				const double middleY = itDubiousChar->y + itDubiousChar->height / 2.0;

				const bool bMiddle = middleY >= lineData.averageY - deltaY && middleY <= lineData.averageY + deltaY;
				const bool bWidth = itDubiousChar->width >= cv::max<double>(0.0, lineData.minWidth - deltaWidth) && itDubiousChar->width <= cv::min<double>(lineData.maxWidth + deltaWidth, plateImage.cols - 1);
				const bool bHeight = itDubiousChar->height >= cv::max<double>(0.0, lineData.minHeight - deltaHeight) && itDubiousChar->height <= cv::min<double>(lineData.maxHeight + deltaHeight, plateImage.rows - 1);

				//const bool bWidth = itDubiousChar->width >= lineData.averageCharWidth - deltaWidth && itDubiousChar->width <= lineData.averageCharWidth + deltaWidth;
				//const bool bHeight = itDubiousChar->height >= lineData.averageCharHeight - deltaHeight && itDubiousChar->height <= lineData.averageCharHeight + deltaHeight;
				
				if (bMiddle && bWidth && bHeight)
				{
					// Debug (show validate dubious char)
					if (_debug > 2)
					{
						cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(0, 255, 0), 2);

						filter::algos::IDPlate::showImage(debugImage);
					}
					// If valid save it and erase it from dubious list to save computing time
					validCharacters.push_back(*itDubiousChar);
					itDubiousChar = dubiousCharacters.erase(itDubiousChar);
					continue;
				}
				else
				{
					// Debug (show invalid dubious char)
					if (_debug > 2)
					{
						cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(0, 0, 255), 2);

						filter::algos::IDPlate::showImage(debugImage);
					}

					//// We also can filter invalid characters which coordinates are above the line.
					//// They can only be noise so we can delete them to speed up computing time
					//if(middleY < lineData.averageY && !bMiddle)
					//{
					//	itDubiousChar = dubiousCharacters.erase(itDubiousChar);
					//	continue;
					//}
				}
			}
			++itDubiousChar;
		}
	}

	// Debug (show valid and newly validated characters)
	if (_debug)
	{
		debugImage = plateImage.clone();
		for (auto & character : validCharacters)
		{
			cv::rectangle(debugImage, character, cv::Scalar(0, 255, 0), 2);
		}
		for (auto & line : charactersLines)
		{
			for(auto & character : line)
			{
				cv::rectangle(debugImage, character, cv::Scalar(0, 255, 0), 1);
			}
		}
		filter::algos::IDPlate::showImage(debugImage);
	}

	// Cleanup used variables
	validCharacters.insert(validCharacters.end(), filteredCharacters.begin(), filteredCharacters.end());
	std::sort(validCharacters.begin(), validCharacters.end(), filter::algos::IDPlate::CompRectsByPos());
	charactersLines = filter::algos::IDPlate::splitCharactersByRows(validCharacters, rowsPositions, plateImage, 0);
	validCharacters.clear();
	charactersLinesData.clear();

	// Recompute each line median Y value
	for (auto & line : charactersLines)
	{
		filter::algos::IDPlate::LineData lineData;
		lineData.minWidth = lineData.minHeight = (std::numeric_limits<int>::max)();
		lineData.maxWidth = lineData.maxHeight = (std::numeric_limits<int>::min)();

		for (auto & character : line)
		{
			lineData.averageY += character.y + character.height / 2.0;
			lineData.averageCharWidth += character.width;
			lineData.averageCharHeight += character.height;

			if (character.width < lineData.minWidth)	lineData.minWidth = character.width;
			if (character.width > lineData.maxWidth)	lineData.maxWidth = character.width;
			if (character.height < lineData.minHeight)	lineData.minHeight = character.height;
			if (character.height > lineData.maxHeight)	lineData.maxHeight = character.height;
		}

		lineData.averageY /= line.size();
		lineData.averageCharWidth /= line.size();
		lineData.averageCharHeight /= line.size();

		charactersLinesData.push_back(lineData);
	}

	// Refilter characters using median value (the ones outside 1st and last character on line)
	for (int lineIdx = 0; lineIdx < charactersLines.size(); ++lineIdx)
	{
		// Assert characters will be between 1st and last rect
		const int minXPos = charactersLines[lineIdx].front().x;
		const int maxXPos = charactersLines[lineIdx].back().x + charactersLines[lineIdx].back().width;

		const filter::algos::IDPlate::LineData& lineData = charactersLinesData[lineIdx];

		// Delta in pixels used to validateCharacters
		const double deltaY = ratioY * lineData.averageCharHeight / 2.0;
		const double deltaHeight = ratioHeight * lineData.averageCharHeight / 2.0;
		const double deltaWidth = ratioWidth * lineData.averageCharWidth / 2.0;

		// Analyze every dubious character
		for (std::vector<cv::Rect>::iterator itDubiousChar = dubiousCharacters.begin(); itDubiousChar != dubiousCharacters.end();)
		{
			// Debug (show min X and max X computed from first and last character of line and show current dubious rect)
			if (_debug > 2)
			{
				debugImage = plateImage.clone();
				// X Bounds
				cv::line(debugImage, cv::Point(minXPos, 0), cv::Point(minXPos, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);
				cv::line(debugImage, cv::Point(maxXPos, 0), cv::Point(maxXPos, debugImage.rows - 1), cv::Scalar(255, 0, 255), 2);

				// Average Y
				cv::line(debugImage, cv::Point(0, lineData.averageY), cv::Point(debugImage.cols - 1, lineData.averageY), cv::Scalar(127, 0, 127), 2);
				cv::line(debugImage, cv::Point(0, lineData.averageY - deltaY), cv::Point(debugImage.cols - 1, lineData.averageY - deltaY), cv::Scalar(255, 0, 255), 1);
				cv::line(debugImage, cv::Point(0, lineData.averageY + deltaY), cv::Point(debugImage.cols - 1, lineData.averageY + deltaY), cv::Scalar(255, 0, 255), 1);

				// Dubious rect
				cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(255, 255, 0), 2);
				// Dubious rect Y
				cv::circle(debugImage, cv::Point(itDubiousChar->x, itDubiousChar->y + itDubiousChar->height / 2.0), 2, cv::Scalar(255, 255, 0), 2);

				filter::algos::IDPlate::showImage(debugImage);
			}
			// This time character must be outside first and last character of line
			// TODO: must bounds be inclusive or exclusive?
			if (!(itDubiousChar->x >= minXPos && itDubiousChar->x <= maxXPos))
			{
				// Character must be near median Y line and similar to others
				const double middleY = itDubiousChar->y + itDubiousChar->height / 2.0;

				const bool bMiddle = middleY >= lineData.averageY - deltaY && middleY <= lineData.averageY + deltaY;
				const bool bWidth = itDubiousChar->width >= cv::max<double>(0.0, lineData.minWidth - deltaWidth) && itDubiousChar->width <= cv::min<double>(lineData.maxWidth + deltaWidth, plateImage.cols - 1);
				const bool bHeight = itDubiousChar->height >= cv::max<double>(0.0, lineData.minHeight - deltaHeight) && itDubiousChar->height <= cv::min<double>(lineData.maxHeight + deltaHeight, plateImage.rows - 1);

				if (bMiddle && bWidth && bHeight)
				{
					// Debug (show validate dubious char)
					if (_debug > 2)
					{
						cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(0, 255, 0), 2);

						filter::algos::IDPlate::showImage(debugImage);
					}
					// If valid save it and erase it from dubious list to speed up compute time
					validCharacters.push_back(*itDubiousChar);
					itDubiousChar = dubiousCharacters.erase(itDubiousChar);
					continue;
				}
				else
				{
					// Debug (show invalid dubious char)
					if (_debug > 2)
					{
						cv::rectangle(debugImage, *itDubiousChar, cv::Scalar(0, 0, 255), 2);

						filter::algos::IDPlate::showImage(debugImage);
					}

					// Here we can safely delete every rect whose coordinates are bove the average line
					if (middleY < lineData.averageY)
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
	if (_debug)
	{
		debugImage = plateImage.clone();
		for (auto & character : validCharacters)
		{
			cv::rectangle(debugImage, character, cv::Scalar(0, 255, 0), 2);
		}
		for (auto & line : charactersLines)
		{
			for (auto & character : line)
			{
				cv::rectangle(debugImage, character, cv::Scalar(0, 255, 0), 1);
			}
		}
		filter::algos::IDPlate::showImage(debugImage);
	}

	//// Split contours in lines
	//// TODO: (last param must be _debug, not 0)
	//std::vector<std::vector<cv::Rect>> rectsLines = filter::algos::IDPlate::splitCharactersByRows(charactersFromContours, rowsPositions, plateImage, 0);

	//// Find average, min, and max character sizes from first filtered characters
	//std::vector<cv::Size> averageCharactersSizes;
	//std::vector<cv::Size> minSizes, maxSizes;

	//for (auto & line : charsLines)
	//{
	//	cv::Size averageSize(0, 0);
	//	cv::Size minSize((std::numeric_limits<int>::max)(), (std::numeric_limits<int>::max)());
	//	cv::Size maxSize((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::min)());

	//	for (auto & character : line)
	//	{
	//		averageSize.width += character.width;
	//		averageSize.height += character.height;

	//		if (character.width < minSize.width)	minSize.width = character.width;
	//		if (character.height < minSize.height)	minSize.height = character.height;

	//		if (character.width > maxSize.width)	maxSize.width = character.width;
	//		if (character.height > maxSize.height)	maxSize.height = character.height;

	//		if (_debug > 2)
	//		{
	//			cv::Mat temp = plateImage.clone();
	//			cv::rectangle(temp, character, cv::Scalar(255, 0, 0), 2);
	//			filter::algos::IDPlate::showImage(temp);
	//		}
	//	}
	//	averageSize.width /= line.size();
	//	averageSize.height /= line.size();

	//	averageCharactersSizes.push_back(averageSize);
	//	minSizes.push_back(minSize);
	//	maxSizes.push_back(maxSize);
	//}
	//if (_debug)	filter::algos::IDPlate::showImage(plateImage);

	//// Use average size + delta to validate characters : KO
	//const cv::Size delta(10, 10);
	//std::vector<std::vector<cv::Rect>> keepedChars(rectsLines.size());
	//for (int i = 0; i < rectsLines.size(); ++i)
	//{
	//	const std::vector<cv::Rect> & currLine = rectsLines[i];
	//	for (int j = 0; j < currLine.size(); ++j)
	//	{
	//		// Debug
	//		if (_debug > 2)
	//		{
	//			cv::Mat temp = plateImage.clone();
	//			cv::rectangle(temp, currLine[j], cv::Scalar(255, 0, 0), 2);
	//			filter::algos::IDPlate::showImage(temp);
	//		}

	//		const cv::Size2d minSize(averageCharactersSizes[i].width - (delta.width / 2.0), averageCharactersSizes[i].height - (delta.height / 2.0));
	//		const cv::Size2d maxSize(averageCharactersSizes[i].width + (delta.width / 2.0), averageCharactersSizes[i].height + (delta.height / 2.0));

	//		const bool width = currLine[j].width >= minSize.width && currLine[j].width < maxSize.width;
	//		const bool height = currLine[j].height >= minSize.height && currLine[j].height < maxSize.height;

	//		if (width && height)
	//		{
	//			keepedChars[i].push_back(currLine[j]);
	//		}
	//	}
	//}

	//// Debug
	//if (_debug)
	//{
	//	cv::Mat wholeTemp = plateImage.clone();
	//	for (auto & line : keepedChars)
	//	{
	//		cv::Mat temp = plateImage.clone();
	//		for (auto & chara : line)
	//		{
	//			cv::rectangle(temp, chara, cv::Scalar(0, 255, 0), 2);
	//			cv::rectangle(wholeTemp, chara, cv::Scalar(0, 255, 0), 2);
	//		}

	//		filter::algos::IDPlate::showImage(temp);
	//	}
	//	filter::algos::IDPlate::showImage(wholeTemp);
	//}

	//// Create buckets
	//const int step = 4;
	//const int bucketCount = plateImage.cols % step ? plateImage.cols / step + 1 : plateImage.cols;


	//std::vector<std::vector<std::vector<cv::Rect>>> bucketsLines(rectsLines.size(), std::vector<std::vector<cv::Rect>>(bucketCount));

	//for (int lineIdx = 0; lineIdx < rectsLines.size(); ++lineIdx)
	//{
	//	const std::vector<cv::Rect>& line = rectsLines[lineIdx];
	//	for (int rectIdx = 0; rectIdx < line.size(); ++rectIdx)
	//	{
	//		const cv::Rect & rect = line[rectIdx];
	//		const int bucketIdx = rect.width / step;
	//		bucketsLines[lineIdx][bucketIdx].push_back(rect);
	//	}
	//}

	//if (_debug)
	//{
	//	for (auto & line : bucketsLines)
	//	{
	//		for (auto & bucket : line)
	//		{
	//			cv::Mat temp = plateImage.clone();
	//			for (auto & rect : bucket)
	//			{
	//				cv::rectangle(temp, rect, cv::Scalar(255, 0, 0), 2);
	//			}
	//			filter::algos::IDPlate::showImage(temp);
	//		}
	//	}
	//}
	//// Extract lines
	//if (_debug)	temp = plateImage.clone();
	//std::vector<std::vector<cv::Rect>> charactersLines = separateCharactersByLines(charactersFromContours, temp);

	//// Debug
	//if (_debug)
	//{
	//	for (auto & line : charactersLines)
	//	{
	//		temp = plateImage.clone();
	//		for (auto & character : line)
	//		{
	//			cv::rectangle(temp, character, cv::Scalar(255, 0, 0), 2);

	//		}
	//		filter::algos::IDPlate::showImage(temp);
	//	}
	//}
	////for (int i = 0; i < contours.size(); ++i)
	////{
	////	// Sort rects
	////	std::sort()
	////	// Extract lines (using y coord) and exclude really small rects
	////	// Segment rects by size (using x coord)
	////	// Find most frequent size (range)
	////	// Use range + offset to validate characters
	////}


}

std::vector<std::vector<cv::Rect>> filter::Algos::IDPlateIdentifier::segmentCharacters(const std::vector<std::vector<cv::Rect>>& lines)
{
	//// We'll segment the rects by their width in ranges of 5 pixels
	//// Find limit (widest rect)
	//int maxWidth = 0;
	//for (auto & line : lines)
	//	for (auto & rect : line)
	//		if (rect.width > maxWidth) maxWidth = rect.width;

	//// If limit is not % 5, add the difference
	//int modulo = maxWidth % 5;
	//if (modulo)
	//{
	//	maxWidth += 5 - modulo;
	//}

	//// Create container
	//int segments = maxWidth / 5;
	//std::vector<std::vector<cv::Rect>>
	//// segment
	return std::vector<std::vector<cv::Rect>>();
}

cv::Mat filter::Algos::IDPlateIdentifier::createOutputImage(const cv::Mat & plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels)
{
	cv::Mat output = plateImage.clone();

	if (charactersRects.size() != charactersLabels.size())
	{
		throw HipeException("[ERROR] IDPlateRectifier::createOutputImage - invalid arguments, charactersRects and charactersLabels sizes don't match.");
	}

	const int fontFace = cv::FONT_HERSHEY_PLAIN;
	const double fontScale = 2;
	const int fontThickness = 2;

	// Print each found texts above plate image
	for (size_t i = 0; i < charactersRects.size(); ++i)
	{
		const cv::Rect& character = charactersRects[i];
		// Find text size
		cv::Size textSize = cv::getTextSize(charactersLabels[i], fontFace, fontScale, fontThickness, nullptr);

		// Text size is computed from the lower left corner of the string
		// We center it on the character rect
		const int posX = (cv::max)(0, (cv::min)(character.x + (character.width - textSize.width) / 2, plateImage.cols - 1));
		const int posY = (cv::max)(0, (cv::min)(character.y + (character.height + textSize.height) / 2, plateImage.rows - 1));
		cv::Point textPos(posX, posY);

		// Print text to image
		cv::putText(output, charactersLabels[i], textPos, fontFace, fontScale, cv::Scalar(0, 0, 255), fontThickness);
	}

	if (_debug)	filter::algos::IDPlate::showImage(output);

	return output;
}

filter::Algos::LabelOCR::LabelOCR()
{
	init();
	_debug = 0;
}

filter::Algos::LabelOCR::LabelOCR(int debugLevel)
	: _debug(debugLevel)
{
	init();
}

filter::Algos::LabelOCR::~LabelOCR()
{
}

void filter::Algos::LabelOCR::preProcess(const cv::Mat & InputImage, cv::Mat & binImage)
{
	//cv::Mat midImage2, dst;

	//cv::Mat Morph = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size(1, 1));
	//cv::Mat HPKernel = (cv::Mat_<float>(5, 5) <<
	//	-1.0, -1.0, -1.0, -1.0, -1.0,
	//	-1.0, -1.0, -1.0, -1.0, -1.0,
	//	-1.0, -1.0, 25.0, -1.0, -1.0,
	//	-1.0, -1.0, -1.0, -1.0, -1.0,
	//	-1.0, -1.0, -1.0, -1.0, -1.0);

	// Color segmentation
	cv::Mat midImage = quantizeImage(InputImage, 2);
	//showImage(midImage);

	//bilateralFilter(midImage, midImage2, ksize, ksize * 2, ksize / 2);
	cv::Mat midImageGrayscale;
	cvtColor(midImage, midImageGrayscale, CV_RGB2GRAY);

	//const int ksize = 11;
	//bilateralFilter(midImage, dst, 3, ksize * 2, ksize / 2);
	//dst = equalize(dst);

	// Output binary image
	binImage = binarizeImage(midImageGrayscale);

	//filter2D(dst, midImage2, InputImage.depth(), HPKernel);
	//cvtColor(midImage2, binImage, COLOR_RGB2GRAY);*/

	//bilateralFilter(InputImage, dst, 15, 15 * 2, 15 / 2);
	//filter2D(dst, midImage2, InputImage.depth(), HPKernel);

	//cvtColor(dst, binImage, CV_BGR2GRAY);

	////create a CLAHE object(Arguments are optional).
	//Ptr<CLAHE> clahe = createCLAHE(2.0);
	//clahe->apply(binImage, binImage);
	////ShowImageWait(small);
	//GaussianBlur(binImage, binImage, Size(3, 3), 0);
	//
	//Mat bw;
	//threshold(binImage, binImage, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	//bitwise_not(binImage, binImage);
	//

	//binImage = InputImage;

	//showImage(binImage);
	//threshold(midImage, binImage, 60, 255, CV_THRESH_BINARY);
	//threshold(binImage, binImage ,0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//erode(binImage, binImage, 3, Point(-1, -1), 2, 1, 1);
	//morphologyEx( binImage,binImage,MORPH_CLOSE, Morph);
}

std::vector<std::string> filter::Algos::LabelOCR::runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType)
{
	std::vector<std::string> output;
	const int minConfidence = 30;

	// Run recognition with Tesseract OCR on each character
	for (size_t i = 0; i < labelsImages.size(); ++i)
	{
		cv::Mat labelImage = labelsImages[i];
		if (!labelImage.empty())
		{
			// Legacy Note: labelType == 1 parameters were: min confidence = 1, component level = OCR_LEVEL_TEXTLINE, labelType == 2 parameters were: min confidene = 30, component level = 0
			std::string result = runPrediction(labelImage, minConfidence, i);
			output.push_back(result);
		}
		else
		{
			output.push_back("error");
			std::cout << "[WARNING] IDPlateIdentifier::LabelOCR - label image is invalid" << std::endl;
		}
	}

	return (output);
}

void filter::Algos::LabelOCR::init()
{
	tesseOCR_ = cv::text::OCRTesseract::create(NULL, "eng", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-Code", 3, 7);
}

std::string filter::Algos::LabelOCR::runPrediction(const cv::Mat & labelImage, int minConfidence, int imageIndex)
{
	cv::Mat image = labelImage.clone();

	// Assert image is valid
	if (!labelImage.data)
	{
		throw HipeException("IDPlateIdentifier::LabelOCR::runPrediction - Input image is invalid");
	}

	// Preprocess image
	cv::Mat preprocessedImage;
	preProcess(image, preprocessedImage);
	const int margin = 20;
	preprocessedImage = enlargeCharacter(preprocessedImage, margin);

	// Find character
	//std::string text = tesseOCR_->run(preprocessedImage, 1, cv::text::OCR_LEVEL_TEXTLINE);	// legacy runPrediction1
	//std::string text = tesseOCR_->run(preprocessedImage, 30);									// legacy runPrediction2
	std::string text = tesseOCR_->run(preprocessedImage, minConfidence);

	// Log the text
	if (imageIndex >= 0)	std::cout << "label_" << imageIndex << ": " << text << std::endl;

	// Debug
	if (_debug > 2)
	{
		cv::Size labelTextSize = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 2, 2, nullptr);
		cv::Mat labelMat = cv::Mat::zeros(preprocessedImage.size(), CV_8UC3);

		const int posX = (cv::max)(0, (cv::min)((labelMat.cols - labelTextSize.width) / 2, labelMat.cols));
		const int posY = (cv::max)(0, (cv::min)((labelMat.rows + labelTextSize.height) / 2, labelMat.rows));
		cv::Point textPos(posX, posY);

		cv::putText(labelMat, text, textPos, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2, 8);

		filter::algos::IDPlate::showImage(labelImage);
		filter::algos::IDPlate::showImage(preprocessedImage);
		filter::algos::IDPlate::showImage(labelMat);
	}

	return text;
}

void filter::Algos::LabelOCR::filterUndesiredChars(std::string & str)
{
	char chars[] = "?";

	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		// you need include <algorithm> to use general algorithms like std::remove()
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

cv::Mat filter::Algos::LabelOCR::quantizeImage(const cv::Mat & image, int clusters, int maxIterations)
{
	// K-Means Segmentation on pixels values
	cv::Mat samples(image.rows * image.cols, 3, CV_32F);

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x * image.rows, z) = image.at<cv::Vec3b>(y, x)[z];

	cv::Mat labels;
	int attempts = 5;
	cv::Mat centers;
	cv::kmeans(samples, clusters, labels, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers);

	cv::Mat output(image.size(), image.type());
	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * image.rows, 0);
			output.at<cv::Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			output.at<cv::Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			output.at<cv::Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
		}

	return output;
}

cv::Mat filter::Algos::LabelOCR::binarizeImage(const cv::Mat & image)
{
	// Output black and white image regarding a threshold
	cv::Mat output;
	cv::threshold(image, output, 230, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	return output;
}

cv::Mat filter::Algos::LabelOCR::enlargeCharacter(const cv::Mat & character, int margin)
{
	// Enlarge character crop
	// Bigger matrix
	const cv::Size newSize(character.cols + margin * 2, character.rows + margin * 2);
	cv::Mat output(cv::Mat::zeros(newSize, character.type()));

	cv::Rect roi(margin, margin, character.cols, character.rows);
	character.copyTo(output(roi));

	if (_debug > 2)	filter::algos::IDPlate::showImage(output);

	// Bigger character
	cv::Mat dilateKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size(3, 3));
	cv::dilate(output, output, dilateKernel);

	if (_debug > 2)	filter::algos::IDPlate::showImage(output);

	return output;
}
