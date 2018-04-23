//@HIPE_LICENSE@
#include <filter/algos/detection/DetectCirclesFilter.h>
#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

HipeStatus filter::algos::DetectCirclesFilter::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat& image = data.Array()[0];

	if (!image.data) throw HipeException("Error - filter::algos::DetectCirclesFilter::process - no input data found.");

	// This is the expected number of cols and rows of the circles grid to find
	int expectedRows = 4;
	int expectedCols = 4;
	std::vector<cv::Vec3f> circles;

	// Detect the circles
	if (!detect_circles(image, circles, expectedRows, expectedCols) && strict)
	{
		std::stringstream errorMessage;
		errorMessage << "WARNING - filter::algos::DetectCirclesFilter::process - Found " << circles.size() << "circles (expected " << expectedRows * expectedCols << ").";
		throw HipeException(errorMessage.str());
	}

	data::ShapeData output;
	output << circles;

	PUSH_DATA(output);

	return OK;
}

bool filter::algos::DetectCirclesFilter::detect_circles(cv::Mat & image, std::vector<cv::Vec3f>& out_circles, int expected_rows, int expected_cols)
{

	int width, height, length;
	cv::Mat blurred_image, gray_image;

	width = image.rows;
	height = image.cols;
	length = std::min(width, height);

	// Blur the image to reduce noise and improve circle detection.
	cv::Size blur_size(width / factor_blur, height / factor_blur);

	cv::blur(image, blurred_image, blur_size);

	// Convert the image to grayscale for the HoughCircles function.
	cv::cvtColor(blurred_image, gray_image, CV_BGR2GRAY);
	blurred_image.release();

	// Use Antilogic's magic numbers.
	float m_dist = length * min_dist;
	int mi_radius = length * min_radius;
	int ma_radius = length * max_radius;
	// Detect the circles.
	// https://docs.opencv.org/2.4/modules/gpu/doc/image_processing.html
	cv::HoughCircles(
		gray_image, // GpuMat
		out_circles, // GpuMat
		CV_HOUGH_GRADIENT, // method
		1, // dp
		m_dist, //minDist
		hough_h_threshold, // cannyThreshold
		hough_l_threshold, // votesThreshold
		mi_radius, // minRadius,
		ma_radius // maxRadius
	);
	gray_image.release();
	// Check that the expected number of circles was detected.
	return (out_circles.size() == (expected_rows * expected_cols));
}
