#pragma once

#include <filter/algos/HideCircles.h>
#include <data/ShapeData.h>
#include <data/ImageData.h>
#include <opencv2/imgproc.hpp>

namespace filter
{
	namespace algos
	{

		HipeStatus HideCircles::process()
		{
			while (!_connexData.empty()) // While i've parent data
			{
				data::ImageData images;
				data::ShapeData circles;

				data::Data input = _connexData.pop();
				if (input.getType() == data::IMGF)
				{
					images = static_cast<data::ImageData>(input);
				}
				else circles = static_cast<data::ShapeData&>(input);

				data::Data input2 = _connexData.pop();
				if (input2.getType() == data::IMGF)
				{
					images = static_cast<data::ImageData>(input2);

				}
				else  circles = static_cast<data::ShapeData&>(input2);


				cv::Mat imageOutput = images.getMat().clone();
				auto s = circles.CirclesArray().size();


				auto width = imageOutput.cols;
				auto height = imageOutput.rows;

				for (auto i(0); i < s; ++i)
				{

					int
						const r = circles.CirclesArray()[i][2] * radius_factor,
						r_inner = r - averaging_width,
						r_outer = r + averaging_width,
						dist_inner = r_inner * r_inner,
						dist_outer = r_outer * r_outer,
						x = circles.CirclesArray()[i][0],
						y = circles.CirclesArray()[i][1],
						x_min = std::max(0, x - r_outer),
						x_max = std::min(width, x + r_outer),
						y_min = std::min(0, y - r_outer),
						y_max = std::max(height, y + r_outer);


					/*
					Collect all pixels on the given perimeter. Restrict the search to the
					bounding box of the outer radius.
					*/
					std::vector<cv::Vec3b> perimeter_pixels;
					for (auto k = x_min; k < x_max; ++k)
					{
						for (auto l = y_min; l < y_max; ++l)
						{
							auto const dx = k - x;
							auto const dy = l - y;
							auto const distance = dx*dx + dy*dy;
							if (distance >= dist_inner && distance <= dist_outer)
							{
								perimeter_pixels.push_back(imageOutput.at<cv::Vec3b>(l, k));
							}
						}
					}

					// Calculate the average color.
					cv::Scalar const mean_color = mean(perimeter_pixels);
					cv::Point2f const center = { circles.CirclesArray()[i][0], circles.CirclesArray()[i][1] };
					cv::circle(imageOutput, center, r_outer, mean_color, -1);
				}


				_connexData.push(data::ImageData(imageOutput));

			}
			return OK;
		}
	}
}
