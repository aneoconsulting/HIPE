#pragma once

#include <filter/data/IOData.h>
#include <opencv2/core/mat.hpp>

namespace filter {
	namespace data {
		class InputData : public filter::data::IOData
		{
		public:
			InputData(IODataType type) : IOData(type)
			{
			}
		};
	}
}
