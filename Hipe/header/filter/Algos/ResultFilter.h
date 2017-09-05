#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/OutputData.h>
#include "data/ConnexData.h"

namespace filter
{
	namespace algos
	{
		/**
		 * \brief [TODO] The ResultFilter filter is the end of graph filter
		 */
		class ResultFilter : public filter::IFilter
		{
			//data::ConnexOutput<data::Data> _connexData;
			CONNECTOR_OUT(data::Data);

			REGISTER(ResultFilter, ())
			{

			}

			REGISTER_P(int, sigma);

			virtual std::string resultAsString() { return "TODO"; };

		public:
			

			HipeStatus process()
			{
				//Copy result data
				//Nothing to do here get the result from scheduler for any other process
				//No children then pop every data from the connector
				if (_childFilters.size() == 0)
				{
					while (_connexData.size() != 0)
					{
						_connexData.pop();
					}
				}
				return OK;
			}

		};

		ADD_CLASS(ResultFilter, sigma);
	}
}
