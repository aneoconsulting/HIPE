#pragma once
#include <pydata/pyData.h>

#include <pydata/pydata_export.h>

namespace pydata
{
	class PYDATA_EXPORT pyDataConverter
	{
		template <typename DataType>
		static pyData* convertTo(DataType& dataIn)
		{
			return nullptr;
		}
	};
}
