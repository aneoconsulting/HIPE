#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeException.h>

namespace filter {namespace Algos {
	class InputData;
}
}

namespace filter
{
	namespace algos
	{
		class Gaussian : public filter::IFilter
		{
			REGISTER(Gaussian, ())
			{

			}

			REGISTER_P(int, sigma);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				throw HipeException(_constructor + " process isn't yet implmented");
			}

			HipeStatus process(data::IOData & outputData)
			{
				throw HipeException("Gaussian is an example. There is no implementetion");
			}

		};

		ADD_CLASS(Gaussian, sigma);
	}
}
