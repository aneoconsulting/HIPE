//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <coredata/OutputData.h>

namespace filter
{
	namespace algos
	{
		/**
		 * \brief The OutputRawData filter will output the data comming from an image in base64
		 */
		class OutputRawDataFilter : public filter::IFilter
		{
			CONNECTOR_OUT(data::OutputData);

			REGISTER(OutputRawDataFilter, ())
			{

			}

			REGISTER_P(int, unused);

			virtual std::string resultAsString() { return "TODO"; };


		public:

			HipeStatus process()
			{
				// Nothing to do here
				// see json::JsonBuilder::buildJson
				// see http::HttpTask
				// see orchestrator::image::DefaultScheduler
				return OK;
			}

		};

		ADD_CLASS(OutputRawDataFilter, unused);
	}
}
