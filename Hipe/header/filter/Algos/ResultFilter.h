#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

#include <istream>
#include <ostream>
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
			

			HipeStatus process(data::IOData & outputData)
			{
				data::OutputData output(_data);
				
				//Copy result data
				data::IOData::downCast<data::OutputData>(outputData) = output;

				return OK;
			}

		};

		ADD_CLASS(ResultFilter, sigma);
	}
}
