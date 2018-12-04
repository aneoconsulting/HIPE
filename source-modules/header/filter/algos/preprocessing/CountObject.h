//@HIPE_LICENSE@
#pragma once
#include <map>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/PatternData.h>
#include <data/ShapeData.h>
#include <data/DirPatternData.h>
#include <core/HipeTimer.h>

namespace filter
{
	namespace algos
	{
		class CountObject : public IFilter
		{
			SET_NAMESPACE("vision/extraction")

			CONNECTOR(data::ShapeData, data::ShapeData);

			REGISTER(CountObject, ()), _connexData(data::INDATA)
			{
				recurrent = 1000.0;
				isStart = false;
			}

			REGISTER_P(double, recurrent);

			cv::Mat saved;
			std::map<std::string, size_t> countTable;
			core::HipeTimer timeout;
			std::atomic<bool> isStart;
			
		public:
			
			bool isShapeData(const data::Data & input);

			HipeStatus process() override;
		};


		ADD_CLASS(CountObject, recurrent) ;
	}
}
