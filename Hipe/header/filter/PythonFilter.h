#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>



namespace filter
{
	namespace algos
	{
		class PythonFilter : public filter::IFilter
		{
			CONNECTOR(data::Data, data::Data);

			REGISTER(PythonFilter, ()), _connexData(data::INOUT)
			{
					
			}

			REGISTER_P(int, unused);

			HipeStatus process() override
			{
				return HipeStatus::OK;
			}


		};

		ADD_CLASS(PythonFilter, unused);
	}
}