#include <filter\Algos\IDPlateIdentifier.h>

HipeStatus filter::Algos::IDPlateIdentifier::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	LabelOCR labelOCR(true);
	std::vector<cv::Mat> characters = detectTextArea(image);

	labelOCR.runRecognition(characters, 2);

	_connexData.push(image);
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

std::vector<cv::Mat> filter::Algos::IDPlateIdentifier::detectTextArea(const cv::Mat & plateImage)
{
	cv::Mat image = plateImage;
	// Preprocess image
	cv::Mat preprocessed = preprocessImage(plateImage);
	cv::Mat binarizedImage;

	//vector<Rect> textAreas = boundingLetters(small, binary, 0.0f, 1.0f);
	std::vector<cv::Rect> characters = findPlateCharacters(preprocessed, 0.0f, 1.0f, binarizedImage);

	cv::Mat charactersBounds = image.clone();
	for (auto & character : characters)
	{
		cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
	}
	showImage(charactersBounds);

	std::vector<cv::Rect> charactersFiltered = sortAndFilterCharacters(characters, plateImage.rows, 0.06, 0.35);

	charactersBounds = image.clone();
	for (auto & character : charactersFiltered)
	{
		cv::rectangle(charactersBounds, character, cv::Scalar(0, 0, 255), 2);
	}
	showImage(charactersBounds);

	charactersFiltered = blobsRectangle(preprocessed, charactersFiltered);
	std::vector<cv::Mat> croppedCharactersImages;

	for (auto & character : charactersFiltered)
	{
		cv::Mat clone(image(character).clone());
		croppedCharactersImages.push_back(clone);

		showImage(clone);
	}

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

	filteredTextAreas.clear();

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
				nextRectCenter.x > lastRect.x && nextRectCenter.x < lastRect.x + nextRect.width + deltaX)
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

	cv::Mat draw = plateImage.clone();
	const int inc = 10;

	for (int i = 0; i < filteredTextAreas.size(); i++)
	{
		filteredTextAreas[i].x = (cv::max)(filteredTextAreas[i].x - inc / 2, 0);
		filteredTextAreas[i].y = (cv::max)(filteredTextAreas[i].y - inc / 2, 0);
		filteredTextAreas[i].width = filteredTextAreas[i].width + inc;
		filteredTextAreas[i].height = filteredTextAreas[i].height + inc;

		filteredTextAreas[i].width = (filteredTextAreas[i].x + filteredTextAreas[i].width) > draw.cols ? filteredTextAreas[i].width - inc : filteredTextAreas[i].width;
		filteredTextAreas[i].height = (filteredTextAreas[i].y + filteredTextAreas[i].height) > draw.rows ? filteredTextAreas[i].height - inc : filteredTextAreas[i].height;

		rectangle(draw, filteredTextAreas[i], cv::Scalar(255, 0, 0), 4);
	}
	showImage(draw);

	return filteredTextAreas;
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

	cv::findContours(imageGrayscale.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

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
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);

		////Debug
		//showImage(mask);

		// ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		//Debug
		cv::rectangle(debugImage, characterRect, cv::Scalar(255, 0, 0), 2);
		//showImage(debugImage);

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
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");

	return 0;
}

cv::Mat filter::Algos::IDPlateIdentifier::convertColorToGrayscale(const cv::Mat & plateImageColor)
{
	cv::Mat output;
	(plateImageColor.channels() != 1) ? cv::cvtColor(plateImageColor, output, CV_BGR2GRAY) : output = plateImageColor.clone();

	//std::cout << "convetColorToGrayscale - Input: Channels(" << plateImageColor.channels() << "). // Type(" << plateImageColor.type() << ")" << std::endl;
	//std::cout << "convetColorToGrayscale - Output: Channels(" << output.channels() << "). // Type(" << output.type() << ")" << std::endl;
	return output; return cv::Mat();
}

cv::Mat filter::Algos::IDPlateIdentifier::convertGrayscaleToBGR(const cv::Mat & plateImage)
{
	cv::Mat output;
	(plateImage.channels() != 3) ? cv::cvtColor(plateImage, output, CV_GRAY2BGR) : output = plateImage.clone();

	//std::cout << "convertGrayscaleToBGR - Input: Channels(" << plateImageGrayscale.channels() << "). // Type(" << plateImageGrayscale.type() << ")" << std::endl;
	//std::cout << "convertGrayscaleToBGR - Output: Channels(" << output.channels() << "). // Type(" << output.type() << ")" << std::endl;
	return output;
}

filter::Algos::LabelOCR::LabelOCR()
{
	init();
	showImages_ = false;
}

filter::Algos::LabelOCR::LabelOCR(bool showImages)
	: showImages_(showImages)
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
	showImage(midImage);

	//bilateralFilter(midImage, midImage2, ksize, ksize * 2, ksize / 2);
	cv::Mat midImageGrayscale;
	cvtColor(midImage, midImageGrayscale, CV_RGB2GRAY);

	//const int ksize = 11;
	//bilateralFilter(midImage, dst, 3, ksize * 2, ksize / 2);
	//dst = equalize(dst);

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

	showImage(binImage);
	//threshold(midImage, binImage, 60, 255, CV_THRESH_BINARY);
	//threshold(binImage, binImage ,0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//erode(binImage, binImage, 3, Point(-1, -1), 2, 1, 1);
	//morphologyEx( binImage,binImage,MORPH_CLOSE, Morph);
}

std::vector<std::string> filter::Algos::LabelOCR::runRecognition(const std::vector<cv::Mat>& labelImage, int labelType)
{
	std::vector<std::string> output;

	output.resize(labelImage.size());

	for (size_t i = 0; i < labelImage.size(); i++)
	{
		if (!labelImage[i].empty() && labelType == 1)
			output[i] = runPrediction1(labelImage[i], i);

		if (!labelImage[i].empty() && labelType == 2)
			output[i] = runPrediction2(labelImage[i], i);
	}
	return (output);
}

void filter::Algos::LabelOCR::init()
{
	tesseOCR_ = cv::text::OCRTesseract::create(NULL, "eng", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-Code", 3, 7);
}

std::string filter::Algos::LabelOCR::runPrediction1(const cv::Mat & labelImage, int i)
{
	std::string t1;
	if (labelImage.empty())	return t1;

	cv::Mat textImage;
	cv::Mat drawImage = labelImage.clone();

	double labelROI_x = labelImage.cols * 0.10;	// initial point x
	double labelROI_y = labelImage.rows * 0.76; // initial point y
	double labelROI_w = labelImage.cols * 0.6;	// width
	double labelROI_h = labelImage.rows * 0.20; // heigth

	cv::Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

	cv::Mat midImage;
	preProcess(drawImage, textImage);

	//tesseOCR_->setWhiteList("W");
	t1 = tesseOCR_->run(textImage, 1, cv::text::OCR_LEVEL_TEXTLINE);
	// Get the text

	/*t1 = string(text1);
	if (t1.size() > 2)
	t1.resize(t1.size() - 2);*/

	std::cout << "label_" << i << ": " << t1 << std::endl;

	if (showImages_)
	{
		cv::putText(drawImage, t1, cv::Point(labelROI.x + 7, labelROI.y - 5), cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 255), 2, 8); // CV_FONT_HERSHEY_SIMPLEX
		cv::rectangle(drawImage, labelROI, cv::Scalar(0, 0, 255), 2, 8, 0);

		std::string iStr = std::to_string(i);

		cv::imshow("label_" + iStr, labelImage);
		cv::imshow("textImage_1_" + iStr, textImage);
		cv::imshow("letters_1_" + iStr, drawImage);
		cv::waitKey(0);
	}

	return t1;
}

std::string filter::Algos::LabelOCR::runPrediction2(const cv::Mat & labelImage, int i)
{
	std::string t1;
	if (labelImage.empty())	return t1;

	cv::Mat textImage;
	cv::Mat drawImage = labelImage.clone();

	double labelROI_x = 0;					// initial point x
	double labelROI_y = 0;					// initial point y
	double labelROI_w = labelImage.cols;	// width
	double labelROI_h = labelImage.rows;	// heigth

	cv::Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

	cv::Mat midImage;
	preProcess(drawImage, textImage);
	drawImage = cv::Mat::zeros(textImage.size(), CV_8UC3);

	//textImage = drawImage;

	t1 = tesseOCR_->run(textImage, 30);
	//tess.TesseractRect(textImage.data, 1, textImage.step1(), labelROI.x, labelROI.y, labelROI.width, labelROI.height);
	// Get the text
	//char* text1 = tess.GetUTF8Text();
	//t1 = string(text1);
	showImage(textImage);


	std::cout << "label_" << i << ": " << t1 << std::endl;

	if (showImages_)
	{
		cv::putText(drawImage, t1, cv::Point(labelROI.x + 7, labelROI.y + 5), cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 255), 2, 8); // CV_FONT_HERSHEY_SIMPLEX
		cv::rectangle(drawImage, labelROI, cv::Scalar(0, 0, 255), 2, 8, 0);

		std::string iStr(std::to_string(i));

		imshow("label_" + iStr, labelImage);
		imshow("textImage_2_" + iStr, textImage);
		imshow("letters_2_" + iStr, drawImage);

		cv::waitKey(0);

		cv::destroyWindow("label_" + iStr);
		cv::destroyWindow("textImage_2_" + iStr);
		cv::destroyWindow("letters_2_" + iStr);
	}

	return (t1);
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

int filter::Algos::LabelOCR::showImage(const cv::Mat & image)
{
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");

	return 0;
}
