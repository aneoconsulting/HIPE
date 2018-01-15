#pragma once
#include <corefilter/tools/RegisterClass.h>

#include <corefilter/IFilter.h>

#include <corefilter/filter_export.h>
#include <opencv2/core/mat.hpp>


namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT PythonFilter : public filter::IFilter
		{
			CONNECTOR(data::Data, data::Data);

			REGISTER(PythonFilter, ()), _connexData(data::INOUT)
			{
				_init = false;
			}

			REGISTER_P(std::string, script_path);
			
			cv::Mat test;

			std::atomic<bool> _init;

			void init_python(const std::string& path);

			std::string GetCurrentWorkingDir(void);

			HipeStatus process() override;	


		};

		ADD_CLASS(PythonFilter, script_path);
	}
}
