#pragma once
#include "corefilter/IFilter.h"

namespace filter
{
	namespace datasource
	{
		class DataSource
		{
		public:
			virtual data::IODataType getSourceType() const = 0;
		};
	}
}

