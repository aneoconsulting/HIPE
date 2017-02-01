#pragma once
#include <filter/tools/RegisterTools.hpp>

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

			void process(Algos::InputData &input)
			{
				
			}

		};

		ADD_CLASS(Gaussian, sigma);
	}
}
