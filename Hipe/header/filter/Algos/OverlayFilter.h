#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/ImageData.h>



namespace filter
{
	namespace algos
	{
		class OverlayFilter : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(OverlayFilter, ()), _connexData(data::INDATA)
			{
				
			}

			REGISTER_P(double, ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				if (_connexData.size() % 2 != 0)
				{
					throw HipeException("The Overlay missing or text data. Please be sure to link properly with parent");
				}
				
				//while (!_connexData.empty()) // While i've parent data
				{
					data::Data data1 = _connexData.pop();
					data::Data data2 = _connexData.pop();

					if (data1.getType() != data::IMGF && 
						data1.getType() != data::TXT_ARR && 
						data1.getType() != data::TXT && 
						data1.getType() != data::SQR_CROP)
					{
						throw HipeException("The Overlay object cant aggregate tan text ATM. Please Develop OverlayFilter");
					}
					if (data2.getType() != data::IMGF &&
						data2.getType() != data::TXT_ARR &&
						data2.getType() != data::TXT &&
						data2.getType() != data::SQR_CROP)
					{
						throw HipeException("The Overlay object cant aggregate tan text ATM. Please Develop OverlayFilter");
					}
					data::ImageData image;
					if (data1.getType() != data::IMGF && data2.getType() != data::IMGF)
					{
						throw HipeException("Missing image to generate overlay text");
					}
					if (data1.getType() == data::IMGF)
						image = static_cast<data::ImageData &>(data1);
					if (data2.getType() == data::IMGF)
						image = static_cast<data::ImageData &>(data1);
					
					if (data1.getType() == data::TXT || data2.getType() == data::TXT_ARR)
						throw HipeException("Text overlay is not yet implemented");

					data::SquareCrop crops;
					if (data1.getType() == data::SQR_CROP)
						crops = static_cast<data::SquareCrop &>(data1);
					if (data2.getType() == data::SQR_CROP)
						crops = static_cast<data::SquareCrop &>(data2);

					for (cv::Rect & rect : crops.getSquareCrop())
					{
						cv::rectangle(image.getMat(), rect, cv::Scalar(255, 0, 0));
					}
					_connexData.push(image);
				}
				return OK;
			}
		};

		ADD_CLASS(OverlayFilter, ratio);
	}
}
