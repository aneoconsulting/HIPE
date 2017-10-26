#pragma once

#include "filter/algos/DrawCircles.h"
#include "data/ShapeData.h"
#include "data/ImageData.h"


namespace filter
{
	namespace algos
	{

		HipeStatus DrawCircles::process()
		{
			

			while (!_connexData.empty()) // While i've parent data
			{

				data::ImageData images;
				data::ShapeData circles;

				data::Data input = _connexData.pop();
				if(input.getType()== data::IMGF)
				{
					 images = input;
				}
				else circles = input; 

				data::Data input2 = _connexData.pop();

				if (input2.getType() == data::IMGF)
				{
					 images = input2;
					 
				}
				else  circles = input2; 
			
				auto const font_scale = factor_font_scale * std::min(images.getMat().rows, images.getMat().cols);
				auto const s = circles.CirclesArray().size();
				cv::Scalar const circle_color(circle_colorR, circle_colorG, circle_colorB);
				cv::Scalar const center_color(center_colorR, center_colorG, center_colorB);
				cv::Scalar const text_color(text_colorR, text_colorG, text_colorB);
			
						
					for (auto i (0); i < s ; ++i)
					{

						cv::Point const center(cvRound(circles.CirclesArray()[i][0]), cvRound(circles.CirclesArray()[i][1]));
						auto  const radius = cvRound(circles.CirclesArray()[i][2]);
						// Draw perimeter.
						circle(images.getMat(), center, radius, circle_color, thickness, 8, 0);
						// Draw center.
						circle(images.getMat(), center, center_radius, center_color, -thickness, 8, 0);
						putText(images.getMat(), std::to_string(i), center, CV_FONT_HERSHEY_PLAIN, font_scale, text_color, 2);
					}
					_connexData.push(images);
			}
			return OK;
		}
	}
}
