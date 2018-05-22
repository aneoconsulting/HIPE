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
#include <core/queue/ConcurrentQueue.h>

#include <coredata/Data.h>
#include <coredata/data_export.h>

namespace data
{
	/**
	* \todo
	* \brief
	*/
	class DataPortBase
	{

	};

	/**
	* \todo
	* \brief Data port contains the data to guarantee the transition with all Ifilter and IModel
	* \tparam D the Data type to transit between 2 and more connexData
	*/
	class DATA_EXPORT DataPort : public DataPortBase
	{
	public:
		core::queue::ConcurrentQueue<Data> data;	//<! The queue used to store the data

		/**
		* \brief Alias to the pop() method. Get the port's next stored data.
		* \see pop()
		* \return Returns the port's next stored data.
		*/
		Data get();

		/**
		* \brief Get the port's next stored data.
		* \return Returns the port's next stored data.
		*/
		Data pop();

		/**
		* \brief Make the port reference data. The port will establish a link to the next filter in the graph and make data exchange possible.
		* \param dataIn The data to reference in the port.
		*/
		void push(Data& dataIn);

		/**
		* \brief Check if there is data present in the port
		* \return Returns true if there is no data in the port
		*/
		bool empty();

		/**
		* \brief Get the number of stored elements in the port
		* \return Returns the number of stored elements in the port
		*/
		size_t size();
	};
}
