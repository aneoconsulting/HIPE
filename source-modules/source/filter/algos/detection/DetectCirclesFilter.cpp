//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
