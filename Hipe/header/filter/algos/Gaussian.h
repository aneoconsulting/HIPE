#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
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
				kernelsize = -1;

			}

			REGISTER_P(int, sigmaX);
			REGISTER_P(int, sigmaY);
			REGISTER_P(int, kernelsize);

		public:
			HipeStatus process() override;

		};

		ADD_CLASS(Gaussian, sigmaX, sigmaY, kernelsize);
	}
}