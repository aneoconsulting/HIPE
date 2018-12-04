//@HIPE_LICENSE@
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
		class FPSResolution : public filter::IFilter
		{
			SET_NAMESPACE("tools/benchmark")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(FPSResolution, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				id = -1;
				count = 0;
				interval_sampling_ms = -1.0;
				avg_elapse = 0;
				isStart = false;
			}

			int id;
			int count;
			core::HipeTimer sampler;
			bool isStart;
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

		ADD_CLASS(FPSResolution, _debug, interval_sampling_ms);


	}
}
