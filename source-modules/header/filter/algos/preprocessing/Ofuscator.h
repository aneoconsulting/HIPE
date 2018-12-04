//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/PatternData.h>
#include <data/ShapeData.h>
#include <data/DirPatternData.h>

namespace filter
{
	namespace algos
	{
		class Obfuscator : public IFilter
		{
			SET_NAMESPACE("vision/extraction")

			CONNECTOR(data::Data, data::ImageData);

			REGISTER(Obfuscator, ()), _connexData(data::INDATA)
			{
				unused = 0;
			}

			REGISTER_P(int, unused);

			cv::Mat saved;

		public:
			
			bool isShapeData(const data::Data & input);

			HipeStatus process() override;
		};


		ADD_CLASS(Obfuscator, unused) ;
	}
}
