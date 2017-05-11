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
			 
			ImageArrayData(data::IODataType type) : IOData(type)
			{
				/*Data::registerInstance(new ImageArrayData());*/
			}

		public:
			using IOData::IOData;

			ImageArrayData() : IOData(SEQIMG)
			{
			}

			

			ImageArrayData(const data::ImageArrayData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
			}

			std::vector<cv::Mat> & Array()
			{
				ImageArrayData &ret = This() ;
				return ret._array;
			}

			const std::vector<cv::Mat> & Array_const() const
			{
				const ImageArrayData &ret = This_const();
				return ret._array;
			}

			ImageArrayData & operator<<(cv::Mat dataMat)
			{
				This()._array.push_back(dataMat);
				return *this;
			}

			virtual void copyTo(const ImageArrayData& left)
			{
				for (const cv::Mat & image: left.Array_const())
				{
					cv::Mat res;
					image.copyTo(res);

					This()._array.push_back(res);
				}
			}

			inline bool empty() const
			{
				return Array_const().empty();
			}
		};
	}
}
