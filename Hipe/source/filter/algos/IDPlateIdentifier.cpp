#include <filter\Algos\IDPlateIdentifier.h>

HipeStatus filter::Algos::IDPlateIdentifier::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	// Find characters
	LabelOCR labelOCR(true);
	std::vector<cv::Rect> charactersRects;
	std::vector<cv::Mat> characters = detectTextArea(image, charactersRects);

	// Identify them using Tesseract
	std::vector<std::string> labels = labelOCR.runRecognition(characters, 30);	// legacy: labelType = 2

	// Output results in an image
	cv::Mat output = createOutputImage(image, charactersRects, labels);

	_connexData.push(output);
	return OK;
}

bool filter::Algos::IDPlateIdentifier::compareRectsByPosition(const cv::Rect & a, const cv::Rect & b)
{
	int ay = (a.y * 0.5);
	int ax = (a.x * 0.5);
	int by = (b.y * 0.5);
	int bx = (b.x * 0.5);

	if (abs(ay - by) > 10)
		return (ay < by);

	return (ax < bx);
}

std::vector<cv::Mat> filter::Algos::IDPlateIdentifier::detectTextArea(const cv::Mat & plateImage, std::vector<cv::Rect> & out_charactersRects)
{
	cv::Mat image = plateImage;
	// Preprocess image
	cv::Mat preprocessed = preprocessImage(plateImage);
	cv::Mat binarizedImage;

	// Find characters
	std::vector<cv::Rect> characters = findPlateCharacters(preprocessed, 0.0f, 1.0f, binarizedImage);

	cv::Mat charactersBounds = image.clone();
	for (auto & character : characters)
	{
		cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
	}
	showImage(charactersBounds);

	// Sort and filter them (how ?)
	std::vector<cv::Rect> charactersFiltered = sortAndFilterCharacters(characters, plateImage.rows, 0.06, 0.35);

	charactersBounds = image.clone();
	for (auto & character : charactersFiltered)
	{
		cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
	}
	showImage(charactersBounds);

	// ?
	//charactersFiltered = blobsRectangle(preprocessed, charactersFiltered);
	//std::vector<std::vector<cv::Rect>> debugChars = separateCharactersByLines(charactersFiltered, plateImage);
	std::vector<cv::Mat> croppedCharactersImages;

	for (auto & character : charactersFiltered)
	{
		cv::Mat clone(image(character).clone());
		croppedCharactersImages.push_back(clone);

		showImage(clone);
	}

	out_charactersRects = charactersFiltered;
	return croppedCharactersImages;
}

cv::Mat filter::Algos::IDPlateIdentifier::preprocessImage(const cv::Mat & plateImage)
{
	cv::Mat image = plateImage.clone();

	//// Downsample image
	//cv::pyrDown(image, image);

	showImage(image);

	int diameter = 15;
	double sigmaColor = 15.0 * 2.0;
	double sigmaSpace = 15.0 * 0.5;

	bool useGPU = false;
	if (!useGPU)
	{
		cv::bilateralFilter(image.clone(), image, diameter, sigmaColor, sigmaSpace);
	}
	else
	{
		if (cv::cuda::getCudaEnabledDeviceCount() > 0)
		{
			cv::cuda::GpuMat cuOutput;
			cuOutput.upload(plateImage);

			cv::cuda::bilateralFilter(cuOutput.clone(), cuOutput, diameter, sigmaColor, sigmaSpace);

			cuOutput.download(image);
		}
		else
		{
			//throw HipeException("Use GPU is set to true but no enabled CUDA GPU was found.");
		}
	}

	image = convertColorToGrayscale(image);

	// Create a CLAHE object(Arguments are optional).
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0);
	clahe->apply(image, image);
	showImage(image);

	//cv::GaussianBlur(image, image, cv::Size(3, 3), 0);
	cv::bitwise_not(image, image);
	showImage(image);

	return image;
}

std::vector<cv::Rect> filter::Algos::IDPlateIdentifier::sortAndFilterCharacters(std::vector<cv::Rect>& characters, int plateImageRows, double lowerBound, double upperBound)
{
	std::vector<cv::Rect> charactersFiltered;

	// Sort characters
	std::sort(characters.begin(), characters.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compareRectsByPosition(a, b); });

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

std::vector<cv::Rect> filter::Algos::IDPlateIdentifier::blobsRectangle(const cv::Mat & plateImage, std::vector<cv::Rect>& characters)
{
	std::vector<cv::Rect> filteredTextAreas;

	// Tri des rects (ici déjà triés)
	std::sort(characters.begin(), characters.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compareRectsByPosition(a, b); });

	//// Concatenation caractères + caractères filtrés ? Manque quelque chose ?
	//for (int i = 0; i < filteredTextAreas.size(); i++)
	//	characters.push_back(filteredTextAreas[i]);
	//filteredTextAreas.clear();

	const int deltaY = 4;
	const double deltaXRatio = 0.60;

	// Pour chaque caractères de la liste
	for (int i = 0, j = 0; i < characters.size() && j < characters.size(); i++)
	{
		// Ajouter caractère à liste
		filteredTextAreas.push_back(characters[i]);

		// référence du rect en fin de liste
		cv::Rect& lastRect = filteredTextAreas.back();

		// Point au centre de sa ligne de droite
		cv::Point lastRectCenter = cv::Point(lastRect.x + lastRect.width, lastRect.y + lastRect.height / 2);


		// Debug
		cv::Mat temp = convertGrayscaleToBGR(plateImage);
		// Current character (i)
		cv::rectangle(temp, characters[i], cv::Scalar(255, 0, 0), 1);
		// Last character (size-1)
		cv::rectangle(temp, lastRect, cv::Scalar(255, 0, 0), 1);
		// Last character right line center
		cv::circle(temp, lastRectCenter, 2, cv::Scalar(0, 0, 255), 2);
		showImage(temp);


		for (j = i + 1; j < characters.size(); j++)
		{
			// Caractère suivant
			cv::Rect& nextRect = characters[j];

			// Point au centre de sa ligne de gauche
			cv::Point nextRectCenter = cv::Point(nextRect.x, nextRect.y + nextRect.height / 2);

			// Debug
			// Next Character (i + 1)
			cv::rectangle(temp, characters[j], cv::Scalar(255, 255, 0), 1);
			// Next Character left line center
			cv::circle(temp, nextRectCenter, 4, cv::Scalar(255, 0, 255), 2);
			showImage(temp);

			// Consider that 4 pixels or under is the same line of character
			const double deltaX = deltaXRatio * characters[j].width;

			if (abs(nextRectCenter.y - lastRectCenter.y) <= deltaY &&
				nextRectCenter.x > lastRect.x && nextRectCenter.x < lastRect.x + lastRect.width + deltaX)
			{
				// Debug
				cv::Mat temp2 = convertGrayscaleToBGR(plateImage);

				cv::Rect tempRect(lastRect);
				tempRect.x = (cv::min)(characters[j].x, tempRect.x);
				tempRect.y = (cv::min)(characters[j].y, tempRect.y);

				tempRect.width = characters[j].x + characters[j].width - tempRect.x;
				tempRect.height = (cv::max)(characters[j].height, tempRect.height);

				// Draw current rect
				cv::rectangle(temp2, characters[i], cv::Scalar(255, 0, 0), 1);
				// Draw last rect
				cv::rectangle(temp2, lastRect, cv::Scalar(255, 0, 0), 1);
				// Draw tempRect
				cv::rectangle(temp2, tempRect, cv::Scalar(0, 255, 0), 1);
				showImage(temp2);

				continue;
			}
			else
			{
				//cv::rectangle(draw, characters[j], cv::Scalar(0, 0, 255), 1);
				i = j - 1;
				break;
			}
		}
	}

	cv::Mat draw = convertGrayscaleToBGR(plateImage);

	// Debug
	for (auto & character : filteredTextAreas)
	{
		cv::rectangle(draw, character, cv::Scalar(255, 0, 0), 1);
	}
	showImage(draw);

	draw = convertGrayscaleToBGR(plateImage);
	const int inc = 10;
	for (int i = 0; i < filteredTextAreas.size(); i++)
	{
		int oldX = filteredTextAreas[i].x;
		int oldY = filteredTextAreas[i].y;
		int oldWidth = filteredTextAreas[i].width;
		int oldHeight = filteredTextAreas[i].height;

		filteredTextAreas[i].x = (cv::max)(filteredTextAreas[i].x - inc / 2, 0);
		filteredTextAreas[i].y = (cv::max)(filteredTextAreas[i].y - inc / 2, 0);

		filteredTextAreas[i].width = filteredTextAreas[i].width + inc;
		filteredTextAreas[i].height = filteredTextAreas[i].height + inc;

		//// OLD
		//filteredTextAreas[i].width = (filteredTextAreas[i].x + filteredTextAreas[i].width) > draw.cols ? filteredTextAreas[i].width - inc : filteredTextAreas[i].width;
		//filteredTextAreas[i].height = (filteredTextAreas[i].y + filteredTextAreas[i].height) > draw.rows ? filteredTextAreas[i].height - inc : filteredTextAreas[i].height;

		// NEW
		int endWidth = filteredTextAreas[i].x + filteredTextAreas[i].width;
		int endHeight = filteredTextAreas[i].y + filteredTextAreas[i].height;
		int newWidth = draw.cols - 1 - filteredTextAreas[i].x;
		int newHeight = draw.rows - 1 - filteredTextAreas[i].y;

		filteredTextAreas[i].width = endWidth > draw.cols - 1 ? newWidth : filteredTextAreas[i].width;
		filteredTextAreas[i].height = endHeight > draw.rows - 1 ? newHeight : filteredTextAreas[i].height;

		// Debug
		cv::rectangle(draw, filteredTextAreas[i], cv::Scalar(255, 0, 0), 4);
	}
	// Debug
	showImage(draw);

	return filteredTextAreas;
}

std::vector<std::vector<cv::Rect>> filter::Algos::IDPlateIdentifier::separateCharactersByLines(std::vector<cv::Rect>& charactersSorted, const cv::Mat & debugImage)
{
	cv::Mat image = debugImage.clone();

	const int verticalDelta = 10;
	//const int horizontalDelta = 10;

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
		cv::Mat image; // = debugImage.clone();
		if (image.data && debug_)
		{
			cv::rectangle(image, *it, cv::Scalar(255, 0, 0));
			cv::circle(image, itBorder, 2, cv::Scalar(0, 0, 0), 2);
			cv::rectangle(image, *itNext, cv::Scalar(255, 0, 0));
			cv::circle(image, itNextBorder, 2, cv::Scalar(255, 255, 255), 2);
			showImage(image);
		}

		// Compare borders y
		int delta = abs(itBorder.y - itNextBorder.y);
		// if without delta, create new line
		if (delta > verticalDelta)
		{
			//Debug
			if (image.data && debug_)
			{
				cv::rectangle(image, *itNext, cv::Scalar(0, 0, 255));
				showImage(image);
			}

			output.push_back(line);
			line.clear();
		}
		else
		{
			// Debug
			if (image.data && debug_)
			{
				cv::rectangle(image, *itNext, cv::Scalar(0, 255, 0));
				showImage(image);
			}
		}

		it = itNext;
		++itNext;
	}

	// Push last point and last line
	line.push_back(*it);
	output.push_back(line);

	cv::Mat temp = debugImage.clone();
	for (auto & line : output)
	{
		for (auto & rect : line)
		{
			cv::rectangle(temp, rect, cv::Scalar(255, 0, 0), 2);
		}
	}

	showImage(temp);
	return output;
}

std::vector<cv::Rect> filter::Algos::IDPlateIdentifier::findPlateCharacters(const cv::Mat & plateImage, double xMinPos, double xMaxPos, cv::Mat & binarizedImage)
{
	cv::Mat imageCopy = plateImage.clone();
	cv::Mat imageGrayscale = convertColorToGrayscale(imageCopy);

	cv::Mat debugImage = convertGrayscaleToBGR(imageGrayscale);

	//// Preprocess
	// First morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));
	////Debug
	//showImage(imageGrayscale);

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	////Debug
	//showImage(imageGrayscale);

	// Second morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));

	// Output "binarized" plate image
	binarizedImage = imageGrayscale.clone();
	////Debug
	//showImage(imageGrayscale);

	// Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat contoursDebug = imageGrayscale.clone();
	cv::findContours(contoursDebug, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	showImage(contoursDebug);

	//// Find characters
	std::vector<cv::Rect> charactersRects;
	cv::Mat mask = cv::Mat::zeros(imageGrayscale.size(), CV_8UC1);

	const double lowerBound = 0.20;
	const double upperBound = 0.80;
	const cv::Size minRectSize(8, 20);
	const double maxRectWidth = 0.1 * imageGrayscale.cols;
	xMinPos = xMinPos * imageGrayscale.cols;
	xMaxPos = xMaxPos * imageGrayscale.cols;

	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect characterRect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, characterRect);
		maskROI = cv::Scalar(0, 0, 0);

		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), 4); // Anciennement CV_FILLED

		//// Debug
		//showImage(mask);

		// Ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		//// Debug
		//cv::rectangle(debugImage, characterRect, cv::Scalar(255, 0, 0), 2);
		//showImage(debugImage);

		// Assume that at least 45% of the rect area is filled if it contains text
		// Debug
		bool ratioBool = ratio > lowerBound && ratio < upperBound;
		bool sizeBool = characterRect.height > minRectSize.height && characterRect.width > minRectSize.width;
		bool maxWidthBool = characterRect.width <= maxRectWidth;
		bool minPosBool = characterRect.x >= xMinPos;
		bool maxPosBool = characterRect.x <= xMaxPos;


		//if (ratio > lowerBound && ratio < upperBound
		//	&&
		//	// Constraints on rect size
		//	(characterRect.height > minRectSize.height && characterRect.width > minRectSize.width)
		//	// These two conditions alone are not very robust. better to use something
		//	// like the number of significant peaks in a horizontal projection as a third condition
		//	&&
		//	characterRect.width <= maxRectWidth
		//	&&
		//	// Bug when the image is already cropped
		//	characterRect.x >= xMinPos
		//	&&
		//	characterRect.x <= xMaxPos
		//	)
		if (ratioBool && sizeBool && maxWidthBool && minPosBool && maxPosBool)
		{
			charactersRects.push_back(characterRect);

			//Debug
			cv::rectangle(debugImage, characterRect, cv::Scalar(0, 255, 0), 2);
		}

		else
		{
			//Debug
			cv::rectangle(debugImage, characterRect, cv::Scalar(0, 0, 255), 2);
		}

		////Debug
		//showImage(debugImage);
	}

	//Debug
	showImage(debugImage);

	// Sort found characters by position
	std::sort(charactersRects.begin(), charactersRects.end(), [this](const cv::Rect& a, const cv::Rect& b) { return this->compareRectsByPosition(a, b); });

	return charactersRects;
}

cv::Mat filter::Algos::IDPlateIdentifier::applyMorphTransform(const cv::Mat & image, cv::MorphShapes morphShape, cv::MorphTypes morphType, cv::Size kernelSize)
{
	cv::Mat output;
	cv::Mat morphKernel = cv::getStructuringElement(morphShape, kernelSize);
	cv::morphologyEx(image, output, morphType, morphKernel);

	return output;
}

int filter::Algos::IDPlateIdentifier::showImage(const cv::Mat & image)
{
	if (!debug_) return 0;
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");

	return 0;
}

cv::Mat filter::Algos::IDPlateIdentifier::convertColorToGrayscale(const cv::Mat & plateImageColor)
{
	cv::Mat output;
	if (plateImageColor.channels() == 2)
	{
		std::cout << "[WARNING] IDplateIdentifier::convertColorToGrayscale - Alpha is not handled";
	}
	(plateImageColor.channels() != 1) ? cv::cvtColor(plateImageColor, output, CV_BGR2GRAY) : output = plateImageColor.clone();

	return output; return cv::Mat();
}

cv::Mat filter::Algos::IDPlateIdentifier::convertGrayscaleToBGR(const cv::Mat & plateImage)
{
	cv::Mat output;
	if (plateImage.channels() == 4)
	{
		std::cout << "[WARNING] IDplateIdentifier::convertGrayscaleToBGR - Alpha is not handled";
	}
	(plateImage.channels() != 3) ? cv::cvtColor(plateImage, output, CV_GRAY2BGR) : output = plateImage.clone();

	return output;
}

cv::Mat filter::Algos::IDPlateIdentifier::createOutputImage(const cv::Mat & plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels)
{
	cv::Mat output = plateImage.clone();

	if (charactersRects.size() != charactersLabels.size())
	{
		throw HipeException("[ERROR] IDPlateRectifier::createOutputImage - invalid arguments, charactersRects and charactersLabels size don't match.");
	}

	const int fontFace = cv::FONT_HERSHEY_PLAIN;
	const double fontScale = 2;
	const int fontThickness = 2;
	int baseline = 0;

	for (size_t i = 0; i < charactersRects.size(); ++i)
	{
		const cv::Rect& character = charactersRects[i];
		cv::Size textSize = cv::getTextSize(charactersLabels[i], fontFace, fontScale, fontThickness, &baseline);

		const int posX = (cv::max)(0, (cv::min)(character.x + (character.width - textSize.width) / 2, plateImage.cols - 1));
		const int posY = (cv::max)(0, (cv::min)(character.y + (character.height + textSize.height) / 2, plateImage.rows - 1));
		cv::Point textPos(posX, posY);

		cv::putText(output, charactersLabels[i], textPos, fontFace, fontScale, cv::Scalar(0, 0, 255), fontThickness);
	}

	showImage(output);

	return output;
}

filter::Algos::LabelOCR::LabelOCR()
{
	init();
	debug_ = false;
}

filter::Algos::LabelOCR::LabelOCR(bool showImages)
	: debug_(showImages)
{
	init();
}

filter::Algos::LabelOCR::~LabelOCR()
{
}

void filter::Algos::LabelOCR::preProcess(const cv::Mat & InputImage, cv::Mat & binImage)
{
	//cv::Mat midImage2, dst;

	cv::Mat Morph = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size(1, 1));
	cv::Mat HPKernel = (cv::Mat_<float>(5, 5) <<
		-1.0, -1.0, -1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0, -1.0, -1.0,
		-1.0, -1.0, 25.0, -1.0, -1.0,
		-1.0, -1.0, -1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0, -1.0, -1.0);

	cv::Mat midImage = quantizeImage(InputImage, 2);
	//showImage(midImage);

	//bilateralFilter(midImage, midImage2, ksize, ksize * 2, ksize / 2);
	cv::Mat midImageGrayscale;
	cvtColor(midImage, midImageGrayscale, CV_RGB2GRAY);

	//const int ksize = 11;
	//bilateralFilter(midImage, dst, 3, ksize * 2, ksize / 2);
	//dst = equalize(dst);

	binImage = binarizeImage(midImageGrayscale);

	const int margin = 20;


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

	for (size_t i = 0; i < labelsImages.size(); ++i)
	{
		cv::Mat labelImage = labelsImages[i];
		if (!labelImage.empty())
		{
			// Legacy Note: labelType was 1 (min confidence = 1, component level = OCR_LEVEL_TEXTLINE), or 2 (min confidene = 30, component level = 0)
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
	if (debug_)
	{
		cv::Size labelTextSize = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 2, 2, nullptr);
		cv::Mat labelMat = cv::Mat::zeros(preprocessedImage.size(), CV_8UC3);

		const int posX = (cv::max)(0, (cv::min)((labelMat.cols - labelTextSize.width) / 2, labelMat.cols));
		const int posY = (cv::max)(0, (cv::min)((labelMat.rows + labelTextSize.height) / 2, labelMat.rows));
		cv::Point textPos(posX, posY);

		cv::putText(labelMat, text, textPos, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2, 8);

		showImage(labelImage);
		showImage(preprocessedImage);
		showImage(labelMat);
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
	cv::Mat output;
	cv::threshold(image, output, 230, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	return output;
}

cv::Mat filter::Algos::LabelOCR::enlargeCharacter(const cv::Mat & character, int margin)
{
	const cv::Size newSize(character.cols + margin * 2, character.rows + margin * 2);
	cv::Mat output(cv::Mat::zeros(newSize, character.type()));

	cv::Rect roi(margin, margin, character.cols, character.rows);
	character.copyTo(output(roi));

	//showImage(output);

	cv::Mat dilateKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size(3, 3));
	cv::dilate(output, output, dilateKernel);

	//showImage(output);

	return output;
}

int filter::Algos::LabelOCR::showImage(const cv::Mat & image)
{
	if (!debug_) return 0;
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");

	return 0;
}
