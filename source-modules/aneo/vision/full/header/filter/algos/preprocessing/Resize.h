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
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		 /**
		 * \var Resize::ratio
		 * The factor by which the image must be rescaled.
		 *
		 * \var Resize::width
		 * If a width and height is provided the image will be rescaled to these values. The width to which the image must be rescaled.
		 *
		 * \var Resize::height
		 * If a width and height is provided the image will be rescaled to these values. The height to which the image must be rescaled.
		 *
		 * \brief The Resize filter will resize an image (downscale or upscale).
		 */
		class Resize : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			SET_NAMESPACE("vision/preprocessing")

			REGISTER(Resize, ()), _connexData(data::INDATA)
			{
				ratio = 1;
				width = 0;
				height = 0;
				inoutData = true;
			}

			REGISTER_P(double, ratio);
			REGISTER_P(int, width);
			REGISTER_P(int, height);
			REGISTER_P(bool, inoutData);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process();
		};

		ADD_CLASS(Resize, ratio, width, height, inoutData) ;
	}
}
