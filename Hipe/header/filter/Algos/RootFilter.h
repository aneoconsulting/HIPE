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

			HipeStatus process(std::shared_ptr<filter::data::IOData> & outputData)
			{
				outputData.reset(&_data, [](filter::data::IOData*){});

				return HipeStatus::OK;
			}
		};

		ADD_CLASS(RootFilter, a);

	}
}
