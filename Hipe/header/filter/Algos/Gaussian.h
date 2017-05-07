#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>


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

			

		};

		ADD_CLASS(Gaussian, sigma);
	}
}
