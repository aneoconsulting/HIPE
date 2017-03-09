#pragma once

#include <filter/data/InputData.h>

namespace filter {
	namespace Algos {
		class InputArrayData : public filter::data::InputData
		{
		public:
			InputArrayData(data::IODataType type) : InputData(type)
			{
			}
		};
	}
}
