#include <filter/algos/ExctractSubImage.h>
namespace filter
{
	namespace algos
	{
		HipeStatus ExctractSubImage::process()
		{

		}
		std::vector<cv::Mat>
			extract_subimages(
				cv::Mat & image,
				bool center_circles = true,
				cv::Scalar line_color = { 128,128,128 },
				cv::Scalar circle_color = { 240,176,0 },
				int circle_thickness = 5,
				float font_scale = 0.8,
				int font_thickness = 1,
				cv::Scalar text_color = { 0,0,255 }
			)
		{
			int
				subwidth = image.cols / cols,
				subheight = image.rows / rows,
				half_subwidth = subwidth / 2,
				half_subheight = subheight / 2;

			cv::Size subimg_size(subwidth, subheight);
			// For calculating overlaps.
			cv::Rect img_rect({ 0,0 }, image.size());
			std::vector<cv::Mat> subimgs;

			for (unsigned int i = 0; i < circles.size(); ++i)
			{
				unsigned int
					row = i / rows,
					col = i % cols;
				cv::Point
					subimg_origin,
					center(circles[i][0], circles[i][1]),
					relative_center;
				cv::Mat subimg(subwidth, subheight, image.type(), cv::Scalar(0));

				if (center_circles)
				{
					subimg_origin.x = center.x - half_subwidth;
					subimg_origin.y = center.y - half_subheight;
				}
				else
				{
					subimg_origin.x = row * subwidth;
					subimg_origin.y = col * subheight;
				}

				relative_center = center - subimg_origin;

				cv::Rect subimg_rect(subimg_origin, subimg_size);

				// Restrict to overlapping region.
				subimg_rect &= img_rect;

				// Copy overlapping area of image to subimage.
				image(subimg_rect).copyTo(subimg(subimg_rect - subimg_origin));


				// Bounding box parameters.
				int
					r = circles[i][2],
					top = center.y + r - subimg_origin.y,
					bottom = center.y - r - subimg_origin.y,
					left = center.x - r - subimg_origin.x,
					right = center.x + r - subimg_origin.x,
					half_bottom = bottom / 2,
					half_left = left / 2,
					half_top = top + (subheight - top) / 2,
					half_right = right + (subwidth - right) / 2;

				std::vector<int>
					vlines{ half_left, left, right, half_right },
					hlines{ half_bottom, bottom, top, half_top };

				cv::Mat color_subimg;

				cvtColor(subimg, color_subimg, CV_GRAY2BGR);
				clean_borders(color_subimg, relative_center);

				for (int vline : vlines)
				{
					line(color_subimg, { vline, 0 }, { vline, subheight - 1 }, line_color);
				}
				for (int hline : hlines)
				{
					line(color_subimg, { 0, hline }, { subwidth - 1, hline }, line_color);
				}
				circle(color_subimg, relative_center, r, circle_color, circle_thickness);

				/*
				TODO
				Remove this later when the parameters are extracted for machine
				learning.
				*/
				// Count pixels in the delimited zones.
				vlines.push_back(subwidth);
				hlines.push_back(subheight);
				int left_border = 0;
				for (int vline : vlines)
				{
					int bottom_border = 0;
					for (int hline : hlines)
					{
						cv::Rect zone{ cv::Point{ left_border, bottom_border }, cv::Size{ vline - left_border, hline - bottom_border } };
						int total_pixels = (hline - bottom_border) * (vline - left_border);
						int black_pixels = total_pixels - countNonZero(subimg(zone));

						// TODO: remove or improve, this is currently just a hack for demonstration
						std::stringstream percent;
						percent << fixed << std::setprecision(1) << (100.0 * black_pixels / total_pixels) << "%";

						subimg_add_text(color_subimg, std::to_string(black_pixels), -1, hline, bottom_border, left_border, font_scale, font_thickness, text_color);
						subimg_add_text(color_subimg, std::to_string(total_pixels), 0, hline, bottom_border, left_border, font_scale, font_thickness, text_color);
						subimg_add_text(color_subimg, percent.str(), 1, hline, bottom_border, left_border, font_scale, font_thickness, text_color);
						bottom_border = hline;
					}
					left_border = vline;
				}
				subimgs.push_back(color_subimg);
				subimg.release();
			}
			return subimgs;
		}


		void
			subimg_add_text(
				cv::Mat & image,
				std::string txt,
				int offset,
				int hline,
				int bottom_border,
				int left_border,
				float font_scale,
				int font_thickness,
				cv::Scalar text_color
			)
		{
			int y;
			cv::Size text_size = cv::getTextSize(txt, cv::FONT_HERSHEY_PLAIN, font_scale, font_thickness, 0);
			if (offset == 1)
			{
				y = hline - 3;
			}
			else if (offset == -1)
			{
				y = bottom_border + text_size.height + 3;
			}
			else
			{
				y = (hline + bottom_border + text_size.height) / 2;
			}
			putText(
				image,
				txt,
				cv::Point(left_border, y),
				cv::FONT_HERSHEY_PLAIN,
				font_scale,
				text_color,
				font_thickness
			);
		}


		void	clean_borders(cv::Mat & image, cv::Point & center, float threshold = 0.1, int thickness = 1)
		{
			int max_r = std::min(
				std::min(center.x, image.cols - center.x),
				std::min(center.y, image.rows - center.y)
			);

			cv::Vec3b black_pixel{ 0,0,0 };
			cv::Vec3b white_pixel{ 255,255,255 };

			for (int r = thickness; r < max_r; ++r)
			{
				int
					r2_inner = r - thickness,
					r2_outer = r + thickness;
				r2_inner *= r2_inner;
				r2_outer *= r2_outer;

				unsigned int total = 0, black = 0;

				for (int i = center.x - r; i < center.x + r; ++i)
				{
					for (int j = center.y - r; j < center.y + r; ++j)
					{
						int
							dx = i - center.x,
							dy = j - center.y,
							distance = dx*dx + dy*dy;
						if (distance >= r2_inner && distance <= r2_outer)
						{
							total++;
							if (image.at<cv::Vec3b>(j, i) == black_pixel)
							{
								black++;
							}
						}
					}
				}
				float ratio = (float)black / total;
				if (ratio <= threshold)
				{
					// TODO: find a better way to do this (masking?)
					for (int i = 0; i < image.cols; ++i)
					{
						for (int j = 0; j < image.rows; ++j)
						{
							int
								dx = i - center.x,
								dy = j - center.y,
								distance = dx*dx + dy*dy;
							if (distance >= r2_outer)
							{
								image.at<cv::Vec3b>(j, i) = white_pixel;
							}
						}
					}
					return;
				}
			}
		}
	}
}