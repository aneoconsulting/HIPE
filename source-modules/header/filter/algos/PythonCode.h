//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>

#include <corefilter/IFilter.h>

#include <corefilter/filter_export.h>
#include <data/PyContextData.h>
#include <core/python/pyThreadSupport.h>

#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT PythonCode : public filter::IFilter
		{
			CONNECTOR(data::Data, data::Data);

			REGISTER(PythonCode, ()), _connexData(data::INDATA)
			{
				_init = false;
				mPyUser = nullptr;
				code_impl = "import sys;\n" "import pydata;\n" "def process(data):\n\tprint('Hello HIPE');";
				function_name = "main";
				temp_script_file = GetCurrentWorkingDir() + "/tmp/" + this->getName() + ".py";
			}
			data::PyContextData l_pythonContext;

			REGISTER_P(std::string, code_impl);
			std::string temp_script_file;
			REGISTER_P(std::string, function_name);
			
			REGISTER_P(std::string, jsonParams);

			std::atomic<bool> _init;

			PyInterpreterState* _m_interp;

			PyThreadState* pyThreadState;
			PyExternalUser* mPyUser;

			void add_python_path(const std::string& python_path);

			void initialize_python_paths();

			//Called from parent thread
			void init_python(const std::string& path);

			void push_result(const boost::python::object& object);

			HipeStatus process() override;	

			void dispose()  override;

			bool isPython() { return true; }

			/**
			 * \brief Function to call from parent thread only
			 * to initialize a new pythonInterpreter
			 * \param interp the python interpreter comming from Default Python threadState
			 */
			void onLoad(void* interp) override;

			/**
			 * \brief Function to call from the thread running process method
			 * Here is only to get the python ThreadState
			 * \param pyThreadState 
			 */
			void onStart(void* pyThreadState) override;
		};

		ADD_CLASS(PythonCode, code_impl, function_name, jsonParams);
	}
}
