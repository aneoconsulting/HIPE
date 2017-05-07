#pragma once
#include <filter/data/IODataType.h>

#include <filter/data/IOData.h>
#include <filter/data/ImageArrayData.h>

namespace filter {
	namespace data {
		class ImageData : public IOData<ImageArrayData, ImageData>
		{
			
		protected:
			ImageData(IOData::_Protection priv)
			{
				
			}

		protected:
			
			using IOData::IOData;

			ImageData(IODataType dataType) : IOData(dataType)
			{
				Array().resize(1);
			}

		public:

			ImageData() : IOData(IMGF)
			{
				Data::registerInstance(new ImageData(IOData::_Protection()));

				Array().resize(1);
			}

			ImageData(cv::Mat matrix) : IOData(IMGF)
			{
				Data::registerInstance(new ImageData(IOData::_Protection()));
				Array().resize(1);
				Array()[0] = matrix;
			}

			virtual void copyTo(IOData& left)
			{
				throw HipeException("Not yet implemented");
				if (IOData::getType() != left.getType())
					throw HipeException("Cannot left argument in a ImageData");
			}

			cv::Mat & getMat()
			{
				if (Array().empty()) Array().resize(1);

				return Array()[0];
				
			}
		};
	}
}
