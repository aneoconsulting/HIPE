#include "filter\Algos\IDPlateCropper.h"

HipeStatus filter::algos::IDPlateCropper::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();

	cv::Mat roi = preprocessPlate(image);

	_connexData.push(data::ImageData(roi));

	return OK;
}

cv::Mat filter::algos::IDPlateCropper::preprocessPlate(const cv::Mat & plateImage)
{
	// Bilateral filtering to smooth images and reduce noise
	cv::Mat output = applyBilateralFiltering(plateImage, bfilterPasses, false);

	// Convert image to grayscale
	output = convertToGrayscale(output);

	// Convert image to binary (black/white)
	cv::adaptiveThreshold(output, output, 255, CV_ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, -2);

	// Find edges using canny algorithm
	cv::Canny(output.clone(), output, 450, 100);

	// Dilate image to enlarge found contours
	cv::Mat dilateKernel =
		(cv::Mat_<uchar>(5, 5) <<
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0,
			1, 1, 1, 1, 1,
			0, 0, 1, 0, 0,
			0, 0, 1, 0, 0);
	dilate(output, output, dilateKernel);

	// Color blobs on image and find biggest blob
	cv::Scalar maskColor(64, 0, 0);
	// Debugging
	showImage(output);

	cv::Point biggestBlobPos = maskBlobs(output, maskColor);

	// Debugging
	showImage(output);

	// Paint back biggest blob in white
	cv::floodFill(output, biggestBlobPos, cv::Scalar(255, 255, 255));

	// Paint everything but biggest blob in black
	for (int y = 0; y < output.size().height; y++)
	{
		uchar* row = output.ptr(y);
		for (int x = 0; x < output.size().width; x++)
		{
			cv::Point currentPixel(x, y);
			if (row[x] == maskColor[0] && currentPixel != biggestBlobPos)
			{
				int area = floodFill(output, currentPixel, cv::Scalar(0, 0, 0));
			}
		}
	}

	// Debugging
	showImage(output);

	// Find contours of biggest blob:
	cv::Rect RegionToCrop = cv::boundingRect(output);

	// Add margin
	const double ratio = 0.05;
	int marginWidth = ratio * plateImage.cols;
	int marginHeight = ratio * plateImage.rows;

	RegionToCrop.width += marginWidth;
	RegionToCrop.height += marginHeight;

	// Rescale if coordinates out of image
	if (RegionToCrop.x + RegionToCrop.width > output.cols)	RegionToCrop.width = output.cols - RegionToCrop.x;
	if (RegionToCrop.y + RegionToCrop.height > output.rows) RegionToCrop.height = output.rows - RegionToCrop.y;


	output = plateImage(RegionToCrop);

	// Debugging
	showImage(output);

	return output;
}

cv::Mat filter::algos::IDPlateCropper::applyBilateralFiltering(const cv::Mat & plateImage, int iterations, bool useGPU)
{
	cv::Mat output = plateImage.clone();

	int diameter = 31;
	double sigmaColor = 31.0 * 2.0;
	double sigmaSpace = 31.0 * 0.5;

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

		// Debug
		showImage(output);
	}

	return output;
}

cv::Mat filter::algos::IDPlateCropper::convertToGrayscale(const cv::Mat & plateImageColor)
{
	cv::Mat output;
	(plateImageColor.channels() == 3 || plateImageColor.channels() == 4) ? cv::cvtColor(plateImageColor, output, CV_BGR2GRAY) : output = plateImageColor.clone();

	return output;
}

cv::Point filter::algos::IDPlateCropper::maskBlobs(cv::Mat & plateImageBlackWhite, const cv::Scalar & color)
{
	int maxArea = -1;
	cv::Point maxAreaPos;

	const int threshold = 128;

	// For each pixel, if pixel color is greater than threshold, it's a blob : paint it black. The biggest found area is the whole plate
	for (int y = 0; y < plateImageBlackWhite.size().height; y++)
	{
		const uchar* row = plateImageBlackWhite.ptr(y);
		for (int x = 0; x < plateImageBlackWhite.size().width; x++)
		{
			if (row[x] >= threshold)
			{
				cv::Point pos(x, y);
				int currArea = cv::floodFill(plateImageBlackWhite, pos, color);
				if (currArea > maxArea)
				{
					maxAreaPos = pos;
					maxArea = currArea;

					// Debug
					showImage(plateImageBlackWhite);
				}
			}
		}
	}
	return maxAreaPos;
}

void filter::algos::IDPlateCropper::showImage(const cv::Mat & image)
{
	if (!debug_) return;
	cv::namedWindow("debug");
	cv::imshow("debug", image);
	cv::waitKey(0);
	cv::destroyWindow("debug");
}
