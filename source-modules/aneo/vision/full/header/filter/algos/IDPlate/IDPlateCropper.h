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
#include <opencv2/core.hpp>
#pragma warning(pop)


#include "filter/algos/IDPlate/IDPlateTools.h"


namespace filter
{
	namespace algos
	{
		/**
		 * \var IDPlateCropper::useGPU
		 * Should the GPU be used to apply the bilateral filtering or not.
		 *
		 * \var IDPlateCropper::bfilterPasses
		 * The desired number of times the bilateral filtering should be applied.
		 *
		 * \var IDPlateCropper::_debug
		 * The desired debug level. The default level is 0 (disabled). A higher value will enable more debug informations.
		 */

		/**
		 * \brief The IDPlateCropper filter will try to find an ID plate in an image and crop it.
		 */
		class IDPlateCropper : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(IDPlateCropper, ()), _connexData(data::INDATA)
			{
				useGPU = false;
				bfilterPasses = 2;
				_debug = 0;
			}

			REGISTER_P(bool, useGPU);
			REGISTER_P(int, bfilterPasses);
			REGISTER_P(int, _debug);

		public:
			HipeStatus process() override;

		private:
			/**
			 * \brief Preprocess the plate image by binarizing it then searching for blobs. The biggest one will be the ROI englobing the text area.
			 * \param plateImage The input plate image in color.
			 * \return A cropped color image of the found ROI englobing the text area.
			 */
			cv::Mat processPlateImage(const cv::Mat& plateImage);
		};

		ADD_CLASS(IDPlateCropper, useGPU, bfilterPasses, _debug);
	}
}
