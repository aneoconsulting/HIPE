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

				kernelSize = 3;

			}

			REGISTER_P(int, kernelSize);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Blur, kernelSize);
	}
}