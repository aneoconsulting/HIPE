//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>
#include <data/ShapeData.h>

#include <sstream>

namespace filter
{
	namespace algos
	{
		class DetectCirclesFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ShapeData);

			REGISTER(DetectCirclesFilter, ()), _connexData(data::INDATA)
			{
				min_dist = 0.12;
				min_radius = 0.015;
				max_radius = 0.04;
				factor_blur = 220;
				hough_h_threshold = 40;
				hough_l_threshold = 30;

				strict = false;
			}

			REGISTER_P(double, min_dist);
			REGISTER_P(double, min_radius);
			REGISTER_P(double, max_radius);
			REGISTER_P(int, factor_blur);

			REGISTER_P(int, hough_h_threshold);
			REGISTER_P(int, hough_l_threshold);

			REGISTER_P(bool, strict);

			HipeStatus process() override;
			/*!
			@brief Detect circles in the given image.
			@param image The input image.
			@param out_circles The outputed fond circles.
			@param expected_rows The expected number of rows of circles to find in the image.
			@param expected_cols The expected number of cols of circles to find in the image.
			@return A boolean indicating if the expected number of circles was found.

			Use the Hough transform to detect a 2D array of circles in the image with
			the expected properties such as radius and spacing. The detected circles
			are stored in the `circles` member, with one item for each circle. Each
			item contains the x and y coordinates of the circle's center and its
			radius.
			*/
			bool detect_circles(cv::Mat & image, std::vector<cv::Vec3f>& out_circles, int expected_rows, int expected_cols);
		};

		ADD_CLASS(DetectCirclesFilter, min_dist, min_radius, max_radius, factor_blur, hough_h_threshold, hough_l_threshold, strict);
	}
}
