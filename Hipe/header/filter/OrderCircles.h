#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <data/ShapeData.h>

namespace filter
{
	namespace algo {

		class OrderCircles : public filter::IFilter
		{
			CONNECTOR(data::ShapeData, data::ShapeData);

			REGISTER(OrderCircles, ()), _connexData(data::INDATA)
			{
			}
			REGISTER_P(int, i);

			virtual std::string resultAsString() { return std::string("TODO"); };
		public:
			HipeStatus process();
		};
		ADD_CLASS(OrderCircles, i);
	}
}
