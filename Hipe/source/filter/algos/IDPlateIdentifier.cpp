#include <filter/algos/IDPlateIdentifier.h>

HipeStatus filter::algos::IDPlateIdentifier::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	// Find characters
	LabelOCR labelOCR(true);
	std::vector<cv::Rect> charactersRects;

	// Preprocess image
	cv::Mat preprocessed = preprocessImage(image);
	cv::Mat binarizedImage;

	// Find characters rois
	int maxLines = 2 * minLines - 1;
	std::vector<std::vector<cv::Rect>> characters = filter::algos::IDPlate::extractPlateCharacters(preprocessed, binarizedImage, minXPos, maxXPos, minLines, maxLines, ratioY, ratioMinArea, ratioMaxArea, image, _debug);

	// Crop rois to create separate images
	std::vector <std::vector<cv::Mat>> croppedCharactersLines;
	for(auto & line : characters)
	{
		croppedCharactersLines.push_back(cropROIs(image, line));
	}

	// Identify them using Tesseract
	std::vector < std::vector<std::string>> labelsLines;
	for (auto & line : croppedCharactersLines)
	{
		labelsLines.push_back(labelOCR.runRecognition(line, 30));	// legacy version: labelType = 2
	}

	// Output results in an image
	cv::Mat output = image.clone();
	for(int i = 0; i < croppedCharactersLines.size(); ++i)
	{
		output = createOutputImage(output, characters[i], labelsLines[i]);
	}

	_connexData.push(output);
	return OK;
}

cv::Mat filter::algos::IDPlateIdentifier::preprocessImage(const cv::Mat & plateImage)
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

cv::Mat filter::algos::IDPlateIdentifier::cropROI(const cv::Mat& image, const cv::Rect& roi)
{
	if (!(roi.x > 0 && roi.x + roi.width <= image.cols - 1) || !(roi.y > 0 && roi.y + roi.height <= image.rows - 1))
		throw HipeException("[ERROR] filter::algos::IDPlateIdentifier::cropROI - crop region is out of image bounds");

	return image(roi).clone();
}

std::vector<cv::Mat> filter::algos::IDPlateIdentifier::cropROIs(const cv::Mat& image, const std::vector<cv::Rect>& rois)
{
	std::vector<cv::Mat> output;
	for (auto & roi : rois)
	{
		output.push_back(cropROI(image, roi));
	}

	return output;
}

cv::Mat filter::algos::IDPlateIdentifier::createOutputImage(const cv::Mat & plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels)
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

filter::algos::LabelOCR::LabelOCR()
{
	init();
	_debug = 0;
}

filter::algos::LabelOCR::LabelOCR(int debugLevel)
	: _debug(debugLevel)
{
	init();
}

filter::algos::LabelOCR::~LabelOCR()
{
}

void filter::algos::LabelOCR::preProcess(const cv::Mat & InputImage, cv::Mat & binImage, int debugLevel)
{
	// Color segmentation
	cv::Mat midImage = quantizeImage(InputImage, 2);
	if(debugLevel) filter::algos::IDPlate::showImage(midImage, "LaelOCR::preProcess - midImage");

	cv::Mat midImageGrayscale;
	cvtColor(midImage, midImageGrayscale, CV_RGB2GRAY);
	if (debugLevel) filter::algos::IDPlate::showImage(midImageGrayscale, "LaelOCR::preProcess - midImageGrayscale");

	// Output binary image
	binImage = binarizeImage(midImageGrayscale);
}

std::vector<std::string> filter::algos::LabelOCR::runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType)
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

void filter::algos::LabelOCR::init()
{
	tesseOCR_ = cv::text::OCRTesseract::create(NULL, "eng", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-Code", 3, 7);
}

std::string filter::algos::LabelOCR::runPrediction(const cv::Mat & labelImage, int minConfidence, int imageIndex)
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
	//std::string text = tesseOCR_->run(preprocessedImage, 1, cv::text::OCR_LEVEL_TEXTLINE);	// legacy version runPrediction1
	//std::string text = tesseOCR_->run(preprocessedImage, 30);									// legacy version runPrediction2
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

cv::Mat filter::algos::LabelOCR::quantizeImage(const cv::Mat & image, int clusters, int maxIterations)
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

cv::Mat filter::algos::LabelOCR::binarizeImage(const cv::Mat & image)
{
	// Output black and white image regarding a threshold
	cv::Mat output;
	cv::threshold(image, output, 230, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	return output;
}

cv::Mat filter::algos::LabelOCR::enlargeCharacter(const cv::Mat & character, int margin)
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
