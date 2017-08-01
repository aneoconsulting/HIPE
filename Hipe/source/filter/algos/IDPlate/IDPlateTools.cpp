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

std::vector<cv::Rect> filter::algos::IDPlate::findPlateCharacters(const cv::Mat & plateImage, double xMinPos, double xMaxPos, bool debug, int contoursFillMethod, cv::Mat & binarizedImage)
{
	cv::Mat imageCopy = plateImage.clone();
	cv::Mat imageGrayscale = filter::algos::IDPlate::convertColor2Gray(imageCopy);

	cv::Mat debugImage = filter::algos::IDPlate::convertGray2Color(imageGrayscale);

	//// Preprocess image to improve recognition
	// First morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_ELLIPSE, cv::MorphTypes::MORPH_GRADIENT, cv::Size(3, 3));
	// Debug
	if (debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Convert to black & white
	const int threshold = 64;
	cv::threshold(imageGrayscale, imageGrayscale, threshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Debug
	if (debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Second morphological transform
	imageGrayscale = applyMorphTransform(imageGrayscale, cv::MorphShapes::MORPH_RECT, cv::MorphTypes::MORPH_CLOSE, cv::Size(3, 1));

	// Output "binarized" plate image
	binarizedImage = imageGrayscale.clone();

	// Debug
	if (debug)	filter::algos::IDPlate::showImage(imageGrayscale);

	// Find contours of preprocessed image
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat contoursDebug = imageGrayscale.clone();
	cv::findContours(contoursDebug, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	// Debug
	if (debug)	filter::algos::IDPlate::showImage(contoursDebug);

	//// Find characters
	std::vector<cv::Rect> charactersRects;
	cv::Mat mask = cv::Mat::zeros(imageGrayscale.size(), CV_8UC1);

	const double lowerBound = 0.20;
	const double upperBound = 0.80;
	const cv::Size minRectSize(8, 20);
	const double maxRectWidth = 0.1 * imageGrayscale.cols;
	xMinPos *= imageGrayscale.cols;
	xMaxPos *= imageGrayscale.cols;

	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect characterRect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, characterRect);
		maskROI = cv::Scalar(0, 0, 0);

		// fill the contours (if countoursFillMethod/Thichness is < 0 the shape will be filled, either way it will use the value as a line thickness parametter
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), contoursFillMethod);

		//// Debug
		//if(debug)	filter::algos::IDPlate::showImage(mask);

		// Ratio of non-zero pixels in the filled region
		double ratio = static_cast<double>(cv::countNonZero(maskROI)) / (characterRect.width * characterRect.height);

		//// Debug
		//if (debug)
		//{
		//	cv::rectangle(debugImage, characterRect, cv::Scalar(255, 0, 0), 2);
		//	filter::algos::IDPlate::showImage(debugImage);
		//}

		// Assume that at least 45% of the rect area is filled if it contains text
		//  Extracted if statment for debugging
		bool ratioBool = ratio > lowerBound && ratio < upperBound;
		// Constraints on rect size
		bool sizeBool = characterRect.height > minRectSize.height && characterRect.width > minRectSize.width;
		// These two conditions alone are not very robust. Better use something like
		// the number of significant peaks in a horizontal projection as a third condition
		bool maxWidthBool = characterRect.width <= maxRectWidth;
		// Bug when the image is already cropped
		bool minPosBool = characterRect.x >= xMinPos;
		bool maxPosBool = characterRect.x <= xMaxPos;

		if (ratioBool && sizeBool && maxWidthBool && minPosBool && maxPosBool)
		{
			charactersRects.push_back(characterRect);

			// Debug
			if (debug)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 255, 0), 2);
		}

		// Debug
		else
		{

			if (debug)	cv::rectangle(debugImage, characterRect, cv::Scalar(0, 0, 255), 2);
		}

		////Debug
		//(if(debug)	filter::algos::IDPlate::showImage(debugImage);
	}

	//Debug
	if (debug)	filter::algos::IDPlate::showImage(debugImage);

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
	(colorImage.channels() == 3) ? cv::cvtColor(colorImage, output, CV_BGR2GRAY) : output = colorImage.clone();

	return output;
}

cv::Mat filter::algos::IDPlate::convertGray2Color(const cv::Mat & grayImage)
{
	cv::Mat output;
	if (grayImage.channels() == 2)
	{
		std::cout << "[WARNING] IDplateIdentifier::convertGray2Color - Alpha is not handled";
	}
	(grayImage.channels() == 1) ? cv::cvtColor(grayImage, output, CV_GRAY2BGR) : output = grayImage.clone();

	return output;
}

void filter::algos::IDPlate::showImage(const cv::Mat & image)
{
	cv::namedWindow("debug image");
	cv::imshow("debug image", image);
	cv::waitKey(0);
	cv::destroyWindow("debug image");
}
