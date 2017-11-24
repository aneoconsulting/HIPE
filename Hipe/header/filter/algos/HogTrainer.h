#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/algos/HogTrainer/HogTrainer.rect.h>

#include <data/ImageData.h>

#include <opencv2/imgproc/imgproc.hpp>

namespace filter
{
	namespace algos
	{
		class HogTrainerFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(HogTrainerFilter, ()), _connexData(data::INOUT)
			{

			}
			REGISTER_P(char, unused);


			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();

				HogTrainer ht;
				ht.run();

				return OK;
			}
		};
		ADD_CLASS(HogTrainerFilter, unused);
	}
}