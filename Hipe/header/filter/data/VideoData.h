#pragma once
#include <filter/data/IODataType.h>
#include <filter/filter_export.h>
#include <filter/data/IOData.h>

namespace filter {
	namespace data {
		template <typename Derived>
		class VideoData : public IOData<Data, Derived>
		{
		public:
			using IOData<Data, Derived>::IOData;


			VideoData(IODataType dataType) : IOData(dataType)
			{

			}	
			
			VideoData(const VideoData &data) : IOData(data)
			{

			}

			virtual ~VideoData() {}

			void copyTo(const VideoData& left)
			{
				static_cast<Derived&>(*this).copyTo(static_cast<const Derived&>(left));
			}

			Data newFrame()
			{
				return static_cast<Derived&>(*this).newFrame();
			}

			bool empty() const
			{
				return static_cast<const Derived&>(*this).empty();
			}
		};
	}
}
