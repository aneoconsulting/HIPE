//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>



#pragma warning(push, 0)

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace corefilter
{
	namespace tools
	{
		class PerfTime : public filter::IFilter
		{
			SET_NAMESPACE("tools/benchmark")

			CONNECTOR(data::Data, data::Data);

			REGISTER(PerfTime, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				id = -1;
				count = 0;
			}

			int id;
			int count;

			REGISTER_P(int, _debug);


			HipeStatus process() override;


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose()
			{
			}
		};

		ADD_CLASS(PerfTime, _debug);


	}
}