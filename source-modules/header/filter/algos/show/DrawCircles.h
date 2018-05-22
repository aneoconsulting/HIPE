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

#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
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
