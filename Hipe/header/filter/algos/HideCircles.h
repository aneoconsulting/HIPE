#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageArrayData.h"
#include <vector>
#include "data/ImageData.h"




namespace filter
{
	namespace algos
	{

		class HideCircles : public filter::IFilter
		{

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(HideCircles, ()), _connexData(data::INDATA)
			{
				    radius_factor = 1.3; 
					averaging_width = 2;

			}

			REGISTER_P(float, radius_factor);
			REGISTER_P(int, averaging_width);
		

		public:
			HipeStatus HideCircles::process() override;

		};

		ADD_CLASS(HideCircles, radius_factor, averaging_width);
	}
}
#pragma once
