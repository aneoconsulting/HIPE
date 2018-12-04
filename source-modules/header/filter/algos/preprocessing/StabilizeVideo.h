//@HIPE_LICENSE@
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>


namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT StabilizeVideo : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ShapeData);

			REGISTER(StabilizeVideo, ()), _connexData(data::INDATA)
			{
				unused = 0;
			}

			REGISTER_P(int, unused);

			HipeStatus process() override;

			void onLoad(void *data) override;
		};

		ADD_CLASS(StabilizeVideo, unused) ;
	}
}
