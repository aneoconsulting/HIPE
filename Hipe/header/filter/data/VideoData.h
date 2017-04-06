#pragma once
#include <filter/data/IODataType.h>

#include <filter/data/IOData.h>

namespace filter {
	namespace data {
		class VideoData : public IOData
		{
		protected:
			VideoData(VideoData &data) : IOData(data)
			{

			}
		public:

			VideoData(IODataType type) : IOData(type) {}
		};
	}
}
