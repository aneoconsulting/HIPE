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
	const int bDiameter = 31;
	cv::Mat output = filter::algos::IDPlate::applyBilateralFiltering(plateImage, bfilterPasses, bDiameter, bDiameter * 2, bDiameter * 0.5, _debug, false);

	// Convert image to grayscale
	output = filter::algos::IDPlate::convertColor2Gray(output);

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
	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

	cv::Point biggestBlobPos = maskBlobs(output, maskColor);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

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

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

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

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

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
					if (_debug)	filter::algos::IDPlate::showImage(plateImageBlackWhite);
				}
			}
		}
	}
	return maxAreaPos;
}
