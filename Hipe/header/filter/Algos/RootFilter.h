#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <filter/tools/RegisterTable.h>



namespace filter {
	namespace Algos {
		class RootFilter : public filter::IFilter
		{
			REGISTER(RootFilter, ()) {}
			REGISTER_P(int, a);

			virtual std::string resultAsString() { return std::string("TODO");  };
		};

		ADD_CLASS(RootFilter, a);

	}
}
