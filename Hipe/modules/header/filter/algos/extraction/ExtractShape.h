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
		class ExtractShape : public IFilter
		{
			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(ExtractShape, ()), _connexData(data::INDATA)
			{
			}

			REGISTER_P(char, unused);

			cv::Mat saved;

		public:
			virtual std::string resultAsString() { return std::string("TODO"); };

			data::ImageArrayData extractMask(data::ImageData& img, data::ShapeData& datas);

			data::ImageData extractSourceImageData(data::Data& data);


			bool isDrawableSource(const data::Data& data);

			bool isOverlayData(const data::Data& data);

			HipeStatus process();
		};

		ADD_CLASS(ExtractShape, unused) ;
	}
}
