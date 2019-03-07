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
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>
#include "core/HipeTimer.h"
#include "data/ShapeData.h"
#include "data/ImageData.h"


namespace algos
{
	namespace tools
	{
		class SelectImageByLabel : public filter::IFilter
		{
			SET_NAMESPACE("vision/utils")

			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(SelectImageByLabel, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				id = -1;
				count = 0;
				image_label = "no_name";
				
			}

			int id;
			int count;
		

			REGISTER_P(int, _debug);
			REGISTER_P(std::string, image_label);

		


			HipeStatus process() override
			{
				while (_connexData.size() > 0)
				{
					data::ImageData data = _connexData.pop();
					if (data.getLabel() == image_label)
					{
						PUSH_DATA(data);
						return OK;
					}
				}

				return OK;
			}


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose()
			{
			}
		};

		ADD_CLASS(SelectImageByLabel, _debug, image_label);


	}
}
