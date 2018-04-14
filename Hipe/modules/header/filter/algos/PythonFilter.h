#pragma once
#include <corefilter/tools/RegisterClass.h>

#include <corefilter/IFilter.h>

#include <corefilter/filter_export.h>
#include <data/PyContextData.h>

#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT PythonFilter : public filter::IFilter
		{
			CONNECTOR(data::Data, data::PyContextData);

			REGISTER(PythonFilter, ()), _connexData(data::INDATA)
			{
				_init = false;
			}
			data::PyContextData l_pythonContext;

			REGISTER_P(std::string, script_path);
			REGISTER_P(std::string, function_name);
			
			REGISTER_P(std::string, jsonParams);

			std::atomic<bool> _init;

			void init_python(const std::string& path);

			HipeStatus process() override;	


		};

		ADD_CLASS(PythonFilter, script_path, function_name, jsonParams);
	}
}
