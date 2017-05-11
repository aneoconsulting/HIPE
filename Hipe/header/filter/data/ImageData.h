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


			virtual void copyTo(const ImageData& left)
			{
				if (IOData::getType() != left.getType())
					throw HipeException("Cannot left argument in a ImageData");
				if (left.Array_const().size() > 1)
					throw HipeException("Number of images inside the source doesn't correspond to a ImageData");

				ImageArrayData::copyTo(static_cast<const ImageArrayData &>(left));

			}

			const cv::Mat & getMat()
			{
				if (Array().empty()) This()._array.resize(1);

				return Array()[0];
				
			}

			inline bool empty() const
			{
				return Array_const().empty();
			}
		};
	}
}
