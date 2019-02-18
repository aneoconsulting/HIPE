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
#include <data/PatternData.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \var Akaze::inlier_threshold
		 * The distance threshold used to identify the inliers.[TODO]
		 *
		 * \var Akaze::nn_match_ratio
		 * The ratio used to match the nearests neighbors.
		 */

		 /**
		  * \todo
		  * \brief The Akaze filter will find and match similarities into two images.
		  * 
		  * The Akaze filter is used to find an object on an image in another one using keypoints and the A-Kaze algorithm.
		  * It awaits a PatternData object as input and will output an image containing the computed matching simimarities contoured.
		  */
		class Akaze : public filter::IFilter
		{

			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(Akaze, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };
		public:
			HipeStatus process();
		};

		ADD_CLASS(Akaze, inlier_threshold, nn_match_ratio);
	}
}
