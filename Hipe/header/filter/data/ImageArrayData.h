#pragma once
#include <filter/data/IOData.h>

namespace filter 
{
	namespace data 
	{
		class ImageArrayData : public IOData<Data, ImageArrayData>
		{
		protected:
			std::vector<cv::Mat> _array;

		public:
			using IOData::IOData;

			ImageArrayData() : IOData(SEQIMG)
			{
			}

			ImageArrayData(data::IODataType type) : IOData(type)
			{
				Data::registerInstance(new ImageArrayData());
			}

			ImageArrayData(const data::ImageArrayData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
			}

			std::vector<cv::Mat> & Array()
			{
				ImageArrayData &ret = This();
				return ret._array;
			}

			ImageArrayData & operator<<(cv::Mat dataMat)
			{
				This()._array.push_back(dataMat);
				return *this;
			}
		};
	}
}
