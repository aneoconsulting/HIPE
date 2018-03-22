#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/ListIOData.h>

namespace filter
{
	namespace algos
	{
		class ExctractSubImage : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ImageArrayData);

			REGISTER(ExctractSubImage, ()), _connexData(data::INDATA)
			{
				debug = 0;
				expected_rows = 4;
				expected_cols = 4;
				center_circles = true;
			}

			REGISTER_P(int, debug);
			REGISTER_P(int, expected_rows);
			REGISTER_P(int, expected_cols);
			REGISTER_P(bool, center_circles);

		private:
			cv::Scalar line_color = { 128,128,128 };
			cv::Scalar circle_color = { 240,176,0 };
			int circle_thickness = 5;
			float font_scale = 0.8;
			int font_thickness = 1;
			cv::Scalar text_color = { 0,0,255 };

		public:
			HipeStatus process() override;

		private:
			void subimg_add_text(cv::Mat & image, std::string txt, int offset, int hline, int bottom_border, int left_border, float font_scale, int font_thickness, cv::Scalar text_color);

			void clean_borders(cv::Mat & image, cv::Point & center, float threshold = 0.1, int thickness = 1);
		};

		ADD_CLASS(ExctractSubImage, debug, expected_rows, expected_cols, center_circles);
	}
}
