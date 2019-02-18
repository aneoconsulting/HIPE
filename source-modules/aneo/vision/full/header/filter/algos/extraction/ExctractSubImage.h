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
