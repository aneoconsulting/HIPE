#include <corebench/benchmarker/PerfTime.h>
#include "core/HipeTimer.h"
#include "coredata/PerfTimeData.h"
#include <core/Logger.h>



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
		//Ignore performance when Data is empty because there is no processing
		if (_connexData.empty())
			return OK;

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
		if (!found) throw HipeException("PerfTime EndTimer Object has no PerfData in input. Can't stop the timer without a chrono");
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
			LOG(INFO) << log.str();


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
			LOG(INFO) << log.str();
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
		//Ignore performance when Data is empty because there is no processing
		if (_connexData.empty()) 
			return OK; 

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
