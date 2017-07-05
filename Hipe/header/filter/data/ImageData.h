#pragma once
#include <filter/data/IODataType.h>

#include <filter/data/IOData.h>
#include <filter/data/ImageArrayData.h>

namespace filter {
	namespace data {
		class ImageData : public IOData<ImageArrayData, ImageData>
		{
		public:
			using IOData::IOData;
			
		protected:
			ImageData(IOData::_Protection priv) : IOData(IMGF)
			{
				
			}

			ImageData(IODataType dataType) : IOData(dataType)
			{
				/*Data::registerInstance(new ImageData(IOData::_Protection()));
				This()._type = dataType;
				This()._array.resize(1);*/
			}

		public:

			ImageData() : IOData(IMGF)
			{
				Data::registerInstance(new ImageData(IOData::_Protection()));
				This()._type = IMGF;
				This()._array.resize(1);
			}

			ImageData(cv::Mat matrix) : IOData(IMGF)
			{
				Data::registerInstance(new ImageData(IOData::_Protection()));
				This()._type = IMGF;
				This()._array.resize(1);
				This()._array[0] = matrix;
			}

			ImageData(const ImageData & ref) : IOData(IMGF)
			{
				Data::registerInstance(ref._This);
				This()._type = ref.This_const()._type;
				_decorate = ref._decorate;
				
			}

			virtual ~ImageData()
			{

				IOData::release();
				_array.clear();

			}


			virtual void copyTo(ImageData& left) const
			{
				if (getType() != left.getType())
					throw HipeException("Cannot left argument in a ImageData");
				if (left.Array_const().size() > 1)
					throw HipeException("Number of images inside the source doesn't correspond to a ImageData");

				ImageArrayData::copyTo(static_cast<ImageArrayData &>(left));

			}


			cv::Mat & getMat()
			{
				if (Array_const().empty())
					Array().push_back(cv::Mat(0, 0, 0));

				return Array()[0];

			}

			const cv::Mat & getMat() const
			{
				return Array_const()[0];
				
			}

			inline bool empty() const
			{
				if (Array_const().empty()) return true;

				return (Array_const()[0].empty());
			}

			ImageData& operator=(const Data& left)
			{
				Data::registerInstance(left);
				_type = left.getType();
				_decorate = left.getDecorate();
				
				return *this;
			}
		};
	}
}
