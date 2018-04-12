#include <filter/algos/show/DrawCircles.h>
#include <data/ShapeData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


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

				auto input = _connexData.pop();
				if(input.getType()== data::IMGF)
				{
					images = static_cast<data::ImageData>(input).getMat().clone();
				}
				else circles = static_cast<data::ShapeData&>(input);

				data::Data input2 = _connexData.pop();

				if (input2.getType() == data::IMGF)
				{
					 images = static_cast<data::ImageData>(input2).getMat().clone();					 
				}
				else  circles = static_cast<data::ShapeData&>(input2); 
			
				auto const font_scale = factor_font_scale * std::min(images.getMat().rows, images.getMat().cols);
				auto const s = circles.CirclesArray().size();
				cv::Scalar const circle_color(circle_color_r, circle_color_g, circle_color_b);
				cv::Scalar const center_color(center_color_r, center_color_g, center_color_b);
				cv::Scalar const text_color(text_color_r, text_color_g, text_color_b);
			
						
					for (auto i (0UL); i < s ; ++i)
					{

						cv::Point const center(cvRound(circles.CirclesArray()[i][0]), cvRound(circles.CirclesArray()[i][1]));
						auto  const radius = cvRound(circles.CirclesArray()[i][2]);
						// Draw perimeter.
						cv::circle(images.getMat(), center, radius, circle_color, thickness, 8, 0);
						// Draw center.
						circle(images.getMat(), center, center_radius, center_color, -thickness, 8, 0);
						putText(images.getMat(), std::to_string(i), center, CV_FONT_HERSHEY_PLAIN, font_scale, text_color, 2);
					}
					PUSH_DATA(images);
			}
			return OK;
		}
	}
}
