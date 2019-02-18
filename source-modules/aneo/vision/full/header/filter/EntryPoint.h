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


#pragma once
#include <corefilter/IFilter.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeException.h>
#include <core/HipeStatus.h>
#include <coredata/ConnexData.h>
#include <coredata/NoneData.h>
#include <corefilter/filter_export.h>


namespace filter {
	namespace algos {
		/**
		* \brief The RootFilter filter is the head of graph filter. Needed by every graph as a data entry point.
		*/
		class FILER_EXPORT EntryPoint : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR_IN(data::NoneData);

			SET_NAMESPACE("Example")

			REGISTER(EntryPoint, ())
			{

			}
			REGISTER_P(int, unused);

		public:
			HipeStatus process()
			{
				////No children then pop every data from the connector
				//if (_childFilters.size() == 0)
				//{
				//	while (_connexData.size() != 0)
				//	{
				//		_connexData.pop();
				//	}
				//}
				return OK;
			}

			/**
			* \brief Overloaded << operator. Can be used to add data to the ConnexData port. \see ConnexData
			* \param element the data to add to the connector.
			* \return A reference to the filter.
			*/
			EntryPoint &operator<<(data::Data & element)
			{
				PUSH_DATA(data::NoneData()); //Here COPY constructor data bug with fields
				return *this;
			}

			/**
			* \brief Overloaded << operator. Can be used to add images to the ConnexData port. \see ConnexData
			* \param element the image to add to the connector.
			* \return A reference to the filter.
			*/
			EntryPoint &operator<<(cv::Mat & element)
			{
				
				return *this;
			}
		};

		ADD_CLASS(EntryPoint, unused);

	}
}
