#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageArrayData.h"
#include <vector>
#include "data/ImageData.h"


namespace filter
{
	namespace algos
	{

		class DrawCircles : public filter::IFilter
		{

			//! @brief Scale factor for fonts when drawing text on images.
			static constexpr float FACTOR_FONT_SCALE = 0.004f;

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(DrawCircles, ()), _connexData(data::INDATA)
			{
				thickness = 5;
				center_radius = 3;
				circle_color_r = 0;
				circle_color_g = 255;
				circle_color_b = 0;
				center_color_r = 0;
				center_color_g = 0;
				center_color_b = 255;
				text_color_r = 255;
				text_color_g = 0;
				text_color_b = 0;				
				
				factor_font_scale = FACTOR_FONT_SCALE;
			}

			REGISTER_P(int, thickness);
			REGISTER_P(int, center_radius);
			REGISTER_P(int, circle_color_r);
			REGISTER_P(int, circle_color_g);
			REGISTER_P(int, circle_color_b);
			REGISTER_P(int, center_color_r);
			REGISTER_P(int, center_color_g);
			REGISTER_P(int, center_color_b);
			REGISTER_P(int, text_color_r);
			REGISTER_P(int, text_color_g);
			REGISTER_P(int, text_color_b);
			REGISTER_P(float, factor_font_scale);



		public:
			HipeStatus process() override;
			
			};

			ADD_CLASS(DrawCircles, thickness, center_radius, circle_color_r, circle_color_g, circle_color_b, center_color_r, center_color_g, center_color_b, text_color_r, text_color_g, text_color_b, factor_font_scale);
		}
	}
