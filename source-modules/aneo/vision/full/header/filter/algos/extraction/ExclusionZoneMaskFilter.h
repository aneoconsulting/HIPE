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

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		class ExclusionZoneMaskFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(ExclusionZoneMaskFilter, ()), _connexData(data::INDATA)
			{
				clahe_clip_limit = 1.0;
				denoise_h = 20;
				denoise_template_window_size = 20;
				denoise_search_window_size = 40;
				clahe_tile_grid_size_x = 4;
				clahe_tile_grid_size_y = 4;
			}

			REGISTER_P(double, clahe_clip_limit);
			REGISTER_P(double, denoise_h);
			REGISTER_P(int, denoise_template_window_size);
			REGISTER_P(int, denoise_search_window_size);
			REGISTER_P(int, clahe_tile_grid_size_x);
			REGISTER_P(int, clahe_tile_grid_size_y);

			HipeStatus process() override;

			/*!
			@brief Compute the exclusion zone mask.
			@param image The image for which to compute the mask (in-situ).
			*/
			void compute_exclusion_zone_mask(cv::Mat & image);
		};

		ADD_CLASS(ExclusionZoneMaskFilter, clahe_clip_limit, denoise_h, denoise_template_window_size, denoise_search_window_size, clahe_tile_grid_size_x, clahe_tile_grid_size_y);
	}
}
