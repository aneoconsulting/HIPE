#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>

namespace filter
{
	namespace algos
	{
		class DemuxPattern : public filter::IFilter
		{
			CONNECTOR(data::DirPatternData, data::PatternData);

			REGISTER(DemuxPattern, ()), _connexData(data::INDATA)
			{
				
			}

			

			REGISTER_P(bool, unused);

			HipeStatus process() override;
			
		};

		ADD_CLASS(DemuxPattern, unused);
	}
}
