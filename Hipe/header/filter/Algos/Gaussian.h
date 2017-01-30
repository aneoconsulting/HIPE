#pragma once
#include <filter/tools/RegisterTools.hpp>
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
		};

		ADD_CLASS(Gaussian, sigma);
	}
}
