#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/OutputData.h>

namespace filter
{
	namespace algos
	{
		class ResultFilter : public filter::IFilter
		{
			

			REGISTER(ResultFilter, ())
			{

			}

			REGISTER_P(int, sigma);

			virtual std::string resultAsString() { return "TODO"; };

		public:
			

			HipeStatus process(std::shared_ptr<filter::data::IOData> & outputData)
			{
				//Copy result data
				outputData.reset(new data::OutputData(_data));
				
				return OK;
			}

		};

		ADD_CLASS(ResultFilter, sigma);
	}
}
