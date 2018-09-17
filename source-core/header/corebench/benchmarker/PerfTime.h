//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>
#include "core/HipeTimer.h"


namespace corefilter
{
	namespace tools
	{
		class PerfTime : public filter::IFilter
		{
			SET_NAMESPACE("tools/benchmark")

			CONNECTOR(data::Data, data::Data);

			REGISTER(PerfTime, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				id = -1;
				count = 0;
				interval_sampling_ms = -1.0;
				avg_elapse = 0;
			}

			int id;
			int count;
			core::HipeTimer sampler;
			double avg_elapse;

			REGISTER_P(int, _debug);

			REGISTER_P(double, interval_sampling_ms);


			HipeStatus process() override;


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose()
			{
			}
		};

		ADD_CLASS(PerfTime, _debug, interval_sampling_ms);


	}
}