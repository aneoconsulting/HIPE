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
#include <coredata/IOData.h>
#include <core/HipeTimer.h>


namespace data
{

	class DATA_EXPORT PerfTimeData : public IOData<Data, PerfTimeData>
	{
		core::HipeTimer hipeTimer;
		double sampling;

	public:
		double getSampling() const
		{
			return This_const().sampling;
		}

		void setSampling(const double sampling)
		{
			This().sampling = sampling;
		}

		core::HipeTimer & getHipeTimer() 
		{
			return This().hipeTimer;
		}

	
	private:
		PerfTimeData(IOData::_Protection priv) : IOData(IODataType::TIMER)
		{

		}

	public:
		PerfTimeData() : IOData(IODataType::TIMER)
		{
			Data::registerInstance(new PerfTimeData(IOData::_Protection()));
		}
		PerfTimeData(core::HipeTimer i_hipeTimer) : IOData(IODataType::TIMER)
		{
			Data::registerInstance(new PerfTimeData(IOData::_Protection()));
			This().hipeTimer = i_hipeTimer;

		}

		PerfTimeData(const PerfTimeData &data) : IOData(data._type)
		{
			Data::registerInstance(data._This);

		}

		PerfTimeData & operator=(const Data &data)
		{
			if (data.getType() != IODataType::TIMER)
				throw HipeException("the left assignment differ from timer");
			const PerfTimeData & perfCast = static_cast<const PerfTimeData &>(data);

			Data::registerInstance(perfCast._This);


			return *this;
		}
	};
}
