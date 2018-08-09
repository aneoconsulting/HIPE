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

#include <corebench/benchmarker/PerfTime.h>
#include "core/HipeTimer.h"
#include "coredata/PerfTimeData.h"
#include <core/Logger.h>

core::Logger perfTimeLogger = core::setClassNameAttribute("PerfTime");

HipeStatus corefilter::tools::PerfTime::process()
{
	core::HipeTimer ti;
	core::HipeTimer ti_internal;
	ti.start();
	ti_internal.start();

	core::HipeTimer ti_end;
	const std::string name = getName();

	if (name.find("__end_timer___") != std::string::npos)
	{
		data::PerfTimeData data_timer(ti);
		bool found = false;
		while (_connexData.size() != 0)
		{
			data::Data onePop = _connexData.pop();
			
			if (onePop.getType() == data::IODataType::TIMER)
			{
				if (found) throw HipeException("PerfTime EndTimer Object cannot be linked to several PerfTimer");

				found = true;
				data_timer = onePop;
			}
		}
		if (!found) throw HipeException("PerfTime EndTimer Object has no data in input. Can't stop the timer without a chrono");
		ti = data_timer.getHipeTimer();
		interval_sampling_ms = data_timer.getSampling();

		if (interval_sampling_ms > 0.0 && count == 0)
		{
			
			ti.stop();
			sampler.start();
			count++;
		
			ti_internal.stop();
			ti.timeval_stop.tv_sec -= ti_internal.getElapse().tv_sec;
			ti.timeval_stop.tv_usec -= ti_internal.getElapse().tv_usec;
			avg_elapse += ti.getElapseTimeInMili();
		}
		else if (interval_sampling_ms > 0.0 && interval_sampling_ms < sampler.stop().getElapseTimeInMili())
		{
			ti_internal.stop();
			ti.stop();

			ti.timeval_stop.tv_sec -= ti_internal.getElapse().tv_sec;
			ti.timeval_stop.tv_usec -= ti_internal.getElapse().tv_usec;
			avg_elapse += ti.getElapseTimeInMili();
			size_t pos = name.find("__end_timer___");
			std::string nodeToAnalysis = name;
			nodeToAnalysis.resize(pos);
			std::stringstream log;

			if (count <= 0) count = 1;

			log << nodeToAnalysis << " average took : " << avg_elapse / (double)count << " ms";
			count = 0;
			avg_elapse = 0.0;
			perfTimeLogger << log.str();


		}
		else if (interval_sampling_ms < 0.0) // Real time timer sampling
		{
			ti_internal.stop();
			ti.stop();

			ti.timeval_stop.tv_sec -= ti_internal.getElapse().tv_sec;
			ti.timeval_stop.tv_usec -= ti_internal.getElapse().tv_usec;
			size_t pos = name.find("__end_timer___");
			std::string nodeToAnalysis = name;
			nodeToAnalysis.resize(pos);
			std::stringstream log;
			log << nodeToAnalysis << " took : " << ti.getElapseTimeInMicro() << " us";
			perfTimeLogger << log.str();
		}
		else
		{
			ti_internal.stop();
			ti.stop();

			ti.timeval_stop.tv_sec -= ti_internal.getElapse().tv_sec;
			ti.timeval_stop.tv_usec -= ti_internal.getElapse().tv_usec;
			avg_elapse += ti.getElapseTimeInMili();
			count++;
		}
	}
	else // This a start PerfTimer
	{
		while (_connexData.size() != 0)
			_connexData.pop();

		ti_internal.stop();
		ti.timeval_stop.tv_sec -= ti_internal.getElapse().tv_sec;
		ti.timeval_stop.tv_usec -= ti_internal.getElapse().tv_usec;

		data::PerfTimeData data(ti);
		data.setSampling(interval_sampling_ms);

		PUSH_DATA(data);

	}

	
	return OK;
}
