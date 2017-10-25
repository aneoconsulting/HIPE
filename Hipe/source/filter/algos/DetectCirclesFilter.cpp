#include <filter/algos/DetectCirclesFilter.h>

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

	// We use a cv::Mat object to transfer the circles data
	int rowsCount = 1;
	int colsCount = circles.size();

	cv::Mat circlesMat(rowsCount, colsCount, CV_32FC3);

	// Assert created matrix is continuous to write its data using the right method
	if (circlesMat.isContinuous())
	{
		colsCount *= rowsCount;
		rowsCount = 1;
	}

	// Don't forget each circle is formed by x,y coordinates plus its radius
	const int circleDataCount = 3;

	// copy data to matrix by indexing it by rows
	for (int y = 0; y < rowsCount; ++y)
	{
		float* row = circlesMat.ptr<float>(y);

		for (int x = 0; x < colsCount; ++x)
		{
			for (int z = 0; z < circleDataCount; ++z)
			{
				row[x] = circles[y * rowsCount + x][z];
			}
		}
	}


	data::ImageArrayData output;
	output << image;
	output << circlesMat;

	_connexData.push(output);

	return OK;
}

bool filter::algos::DetectCirclesFilter::detect_circles(Mat& image, std::vector<cv::Vec3f>& out_circles, int expected_rows, int expected_cols)
{

	int width, height, length;
	Mat blurred_image, gray_image;

	width = image.rows;
	height = image.cols;
	length = min(width, height);

	// Blur the image to reduce noise and improve circle detection.
	Size blur_size(width / factor_blur, height / factor_blur);

	blur(image, blurred_image, blur_size);

	// Convert the image to grayscale for the HoughCircles function.
	cvtColor(blurred_image, gray_image, CV_BGR2GRAY);
	blurred_image.release();

	// Use Antilogic's magic numbers.
	float m_dist = length * min_dist;
	int mi_radius = length * min_radius;
	int ma_radius = length * max_radius;
	// Detect the circles.
	// https://docs.opencv.org/2.4/modules/gpu/doc/image_processing.html
	HoughCircles(
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
