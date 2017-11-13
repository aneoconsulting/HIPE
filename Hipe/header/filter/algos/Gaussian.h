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

		class Gaussian : public filter::IFilter
		{

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(Gaussian, ()), _connexData(data::INDATA)
			{
				sigma = 2;

			}

			REGISTER_P(int, sigma);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Gaussian, sigma);
	}
}