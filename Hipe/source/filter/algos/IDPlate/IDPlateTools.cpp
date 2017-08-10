#include <filter/Algos/IDPlate/IDPlateTools.h>


cv::Mat filter::algos::IDPlate::applyBilateralFiltering(const cv::Mat & plateImage, int iterations, int diameter, double sigmaColor, double sigmaSpace, bool debug, bool useGPU)
{
	cv::Mat output = plateImage.clone();

	// Using CPU to compute can take time
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

		//// Debug
		//showImage(output);
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

std::vector<int> filter::algos::IDPlate::splitImgByCharRows(const cv::Mat & image, const std::vector<cv::Rect> & characters)
{
	std::vector<int> lines;

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

		//// Debug
		//if (debug > 2)
		//{
		//	cv::circle(image, charRectCenter, 4, cv::Scalar(255, 255, 0), 4);
		//	showImage(dbgImage);
		//}

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


	//// For each line and the next one
	//for (int j = 0, prevLine = 0, nextLine = rows[j]; j < linesCount; j++)
	//{
	//	nextLine = rows[j];

	//	// Debug
	//	if (debug)	dbgImage = image.clone();

	//	// For each character : if character is between the two lines add it to the correct container
	//	for (int i = 0; i < inout_characters.size(); i++)
	//	{
	//		cv::Point rectCenter;
	//		rectCenter.x = inout_characters[i].x + inout_characters[i].width * 0.5;
	//		rectCenter.y = inout_characters[i].y + inout_characters[i].height * 0.5;

	//		// Debug
	//		if (debug > 2)
	//		{
	//			cv::circle(image, rectCenter, 4, cv::Scalar(255, 255, 0), 4);
	//			showImage(dbgImage);
	//		}

	//		if (rectCenter.y > prevLine && rectCenter.y < nextLine)
	//		{
	//			sortedCharacters[j].push_back(inout_characters[i]);
	//		}
	//	}

	//	prevLine = nextLine;
	//}

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

/**
 * \brief 
 * \param plateImage 
 * \param out_binarizedImage 
 * \param minPosX 
 * \param maxPosX 
 * \param charMinFillRatio 
 * \param charMaxFillRatio 
 * \param charRectMinSize 
 * \param contoursFillMethod 
 * \param debugLevel 
 * \return 
 */
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

		//// Debug
		if (debugLevel > 1)	filter::algos::IDPlate::showImage(mask);

		// Ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		//// Debug
		if (debugLevel > 1)
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
			if (debugLevel > 1)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 255, 0), 2);
		}

		// Debug
		else
		{

			if (debugLevel > 1)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 0, 255), 2);
		}

		//Debug
		if (debugLevel > 1)	filter::algos::IDPlate::showImage(debugImage);
	}

	//Debug
	if (debugLevel)	filter::algos::IDPlate::showImage(debugImage);

	// Sort found characters by position
	//std::sort(charactersRects.begin(), charactersRects.end(), [](const cv::Rect& a, const cv::Rect& b) { filter::algos::IDPlate::compRectsByPos(a, b); });
	std::sort(charactersRects.begin(), charactersRects.end(), CompRectsByPos());

	return charactersRects;
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

cv::Mat filter::algos::IDPlate::downscaleImage(const cv::Mat & image, int ratio)
{
	if (ratio < 1)	throw HipeException("IDPlateTools::downscaleImage - downscale ratio must be positive");

	const cv::Size newSize(image.cols / ratio, image.rows / ratio);

	cv::Mat output;
	cv::resize(image, output, newSize, 0.0, 0.0, cv::INTER_CUBIC);

	return output;
}

void filter::algos::IDPlate::showImage(const cv::Mat & image)
{
	cv::namedWindow("debug image");
	cv::imshow("debug image", image);
	cv::waitKey(0);
	cv::destroyWindow("debug image");
}
