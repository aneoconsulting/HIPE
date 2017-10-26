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
				circle_colorR = 0;
				circle_colorG = 255;
				circle_colorB = 0;
				center_colorR = 0;
				center_colorG = 0;
				center_colorB = 255;
				text_colorR = 255;
				text_colorG = 0;
				text_colorB = 0;
				
				
				factor_font_scale = FACTOR_FONT_SCALE;
			}

			REGISTER_P(int, thickness);
			REGISTER_P(int, center_radius);
			REGISTER_P(int, circle_colorR);
			REGISTER_P(int, circle_colorG);
			REGISTER_P(int, circle_colorB);
			REGISTER_P(int, center_colorR);
			REGISTER_P(int, center_colorG);
			REGISTER_P(int, center_colorB);
			REGISTER_P(int, text_colorR);
			REGISTER_P(int, text_colorG);
			REGISTER_P(int, text_colorB);
			REGISTER_P(float, factor_font_scale);



		public:
			HipeStatus process() override;
			
			};

			ADD_CLASS(DrawCircles, thickness, center_radius, circle_colorR, circle_colorG, circle_colorB, center_colorR, center_colorG, center_colorB, text_colorR, text_colorG, text_colorB, factor_font_scale);
		}
	}
