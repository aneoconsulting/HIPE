//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"

namespace filter
{
	namespace algos
	{

		class Median : public filter::IFilter
		{


			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(Median, ()), _connexData(data::INDATA)
			{
		
				kernelSize = 3;

			}

			REGISTER_P(int, kernelSize);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Median, kernelSize);
	}
}