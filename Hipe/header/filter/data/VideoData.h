#pragma once
#include <filter/data/IODataType.h>
#include <filter/filter_export.h>
#include <filter/data/IOData.h>

namespace filter {
	namespace data {
		class FILTER_EXPORT VideoData : public IOData<Data, VideoData>
		{
		protected:
			using IOData::IOData;

		protected:
			VideoData(VideoData &data) : IOData(data)
			{

			}
		public:

			VideoData(IODataType type) : IOData(type) {}

			virtual ~VideoData() {}
		};
	}
}
