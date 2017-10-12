#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/OutputData.h>

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
				// see filter::data::OutputData::resultAsJson
				// see http::HttpTask
				// see orchestrator::image::DefaultScheduler
				return OK;
			}

		};

		ADD_CLASS(OutputRawDataFilter, unused);
	}
}
