#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/ListIOData.h>

namespace filter
{
	namespace algos
	{
		class ExctractSubImage : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ImageArrayData);

			REGISTER(ExctractSubImage, ()), _connexData(data::INDATA)
			{
			}

			REGISTER_P(char, unused);
			HipeStatus process() override;
		};

		ADD_CLASS(ExctractSubImage, unused);
	}
}
