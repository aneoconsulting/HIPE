#include <filter/data/IODataType.h>

#include <filter/data/IOData.h>

namespace filter {
	namespace data {
		class ImageData : public IOData
		{
		protected:
			ImageData(ImageData &data) : IOData(data)
			{
				
			}
		public:
			
			ImageData(IODataType type) : IOData(type) {}
		};
	}
}
