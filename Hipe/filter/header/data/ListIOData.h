#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <filter/data/IODataType.h>
#include <filter/data/IOData.h>

namespace filter
{
	namespace data
	{
		class ListIOData : public IOData
		{
			std::vector<IOData> listIoData;
		public:
			ListIOData() : IOData(IODataType::LISTIO)
			{
				
			}
		};
	}
}
