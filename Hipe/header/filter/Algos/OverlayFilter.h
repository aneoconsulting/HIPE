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

			REGISTER(OverlayFilter, ()), _connexData(data::INOUT)
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

					if (data1.getType() != data::IMGF && data1.getType() != data::TXT_ARR && data1.getType() != data::TXT)
					{
						throw HipeException("The Overlay object cant aggregate tan text ATM. Please Develop OverlayFilter");
					}
					if (data2.getType() != data::IMGF && data2.getType() != data::TXT_ARR && data2.getType() != data::TXT)
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
					
					//TODO
				}
				return OK;
			}
		};

		ADD_CLASS(OverlayFilter, ratio);
	}
}
