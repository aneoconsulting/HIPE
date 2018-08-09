//@HIPE_LICENSE@
#include <algos/IDPlate/LabelOCR.h>

filter::algos::LabelOCR2::LabelOCR2()
{
	init();
	_debug = 0;
}

filter::algos::LabelOCR2::LabelOCR2(int debugLevel)
	: _debug(debugLevel)
{
	init();
}

filter::algos::LabelOCR2::~LabelOCR2()
{
}

void filter::algos::LabelOCR2::preProcess(const cv::Mat & InputImage, cv::Mat & binImage, int debugLevel)
{
	// Color segmentation
	cv::Mat midImage = quantizeImage(InputImage, 2);
	if (debugLevel) filter::algos::IDPlate::showImage(midImage, "LaelOCR::preProcess - midImage");

	cv::Mat midImageGrayscale;
	cvtColor(midImage, midImageGrayscale, CV_RGB2GRAY);
	if (debugLevel) filter::algos::IDPlate::showImage(midImageGrayscale, "LaelOCR::preProcess - midImageGrayscale");

	// Output binary image
	binImage = binarizeImage(midImageGrayscale);
}

std::vector<std::string> filter::algos::LabelOCR2::runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType)
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
			std::cout << "[WARNING] IDPlateIdentifier::LabelOCR2 - label image is invalid" << std::endl;
		}
	}

	return (output);
}

void filter::algos::LabelOCR2::init()
{
	tesseOCR_ = cv::text::OCRTesseract::create(NULL, "eng", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-Code", 3, 7);
}

std::string filter::algos::LabelOCR2::runPrediction(const cv::Mat & labelImage, int minConfidence, int imageIndex)
{
	cv::Mat image = labelImage.clone();

	// Assert image is valid
	if (!labelImage.data)
	{
		throw HipeException("IDPlateIdentifier::LabelOCR2::runPrediction - Input image is invalid");
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

cv::Mat filter::algos::LabelOCR2::quantizeImage(const cv::Mat & image, int clusters, int maxIterations)
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

cv::Mat filter::algos::LabelOCR2::binarizeImage(const cv::Mat & image)
{
	// Output black and white image regarding a threshold
	cv::Mat output;
	cv::threshold(image, output, 230, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	return output;
}

cv::Mat filter::algos::LabelOCR2::enlargeCharacter(const cv::Mat & character, int margin)
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
