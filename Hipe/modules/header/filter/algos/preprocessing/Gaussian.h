#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"




namespace filter
{
	namespace algos
	{

		class Gaussian : public filter::IFilter
		{

		
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(Gaussian, ()), _connexData(data::INDATA)
			{
				sigmaX = 1;
				sigmaX = sigmaY;
				kernelSize = -1;

			}

			REGISTER_P(int, sigmaX);
			REGISTER_P(int, sigmaY);
			REGISTER_P(int, kernelSize);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Gaussian, sigmaX, sigmaY, kernelSize);
	}
}