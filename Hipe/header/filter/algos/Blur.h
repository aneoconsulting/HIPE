#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"

namespace filter
{
	namespace algos
	{

		class Blur : public filter::IFilter
		{


			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(Blur, ()), _connexData(data::INDATA)
			{

				kernelsize = 3;

			}

			REGISTER_P(int, kernelsize);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Blur, kernelsize);
	}
}