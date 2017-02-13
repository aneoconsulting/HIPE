#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <filter/tools/RegisterTable.h>
#include <core/HipeException.h>



namespace filter {
	namespace Algos {
		class RootFilter : public filter::IFilter
		{
			REGISTER(RootFilter, ()) {}
			REGISTER_P(int, a);

		public:
			HipeStatus process()
			{
				throw HipeException(_constructor + " process isn't yet implmented");
			}

			HipeStatus process(data::IOData & outputData)
			{
				outputData = _data;

				return HipeStatus::OK;
			}
		};

		ADD_CLASS(RootFilter, a);

	}
}
