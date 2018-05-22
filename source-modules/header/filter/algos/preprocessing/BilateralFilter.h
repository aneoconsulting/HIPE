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
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)
#include <string>

namespace filter
{
	namespace algos
	{
		/**
		 * \var BilateralFilter::d
		 * The diameter of each pixel neighborhood to use
		 *
		 * \var BilateralFilter::color
		 * The standard deviation in the color space to apply
		 *
		 * \var BilateralFilter::space
		 * The standard deviation in the coordinate space (in pixel terms) to apply
		 *
		 * \var BilateralFilter::border
		 * The border mode used to extrapolate pixels outside of the image. \see cv::BorderTypes
		 */

		/**
		 * \brief The BilateralFilter filter will smooth the image with the bilateral filtering method.
		 *  
		 *  The smoothing method usedwill not alter the edges of the shapes.
		 *  Mind that the filtering algorithm is performed on the CPU, each pass takes a certain amount of time.
		 *  \see cv::bilateralFilter()
		 */
		class BilateralFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(BilateralFilter, ()), _connexData(data::INDATA)
			{
				border = cv::BorderTypes::BORDER_DEFAULT;
			}
			REGISTER_P(int, d);
			REGISTER_P(double, color);
			REGISTER_P(double, space);
			REGISTER_P(int, border);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] BilateralFilter::process - No input data found.");
				}

				cv::Mat output;
				cv::bilateralFilter(image, output, d, color, space, border);

				PUSH_DATA(data::ImageData(output));
				return OK;
			}
		};
		ADD_CLASS(BilateralFilter, d, color, space, border);
	}
}
