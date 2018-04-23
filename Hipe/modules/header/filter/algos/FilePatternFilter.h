//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>

namespace filter
{
	namespace algos
	{
		class FilePatternFilter : public  IFilter
		{
			CONNECTOR(data::DirPatternData, data::PatternData);

			REGISTER(FilePatternFilter, ()), _connexData(data::INDATA)
			{

			}



			REGISTER_P(std::string, filePath);

			HipeStatus process() override;

		};

		ADD_CLASS(FilePatternFilter, filePath);
	}
}