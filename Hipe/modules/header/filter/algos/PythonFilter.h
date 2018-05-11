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
		class FILTER_EXPORT PythonFilter : public filter::IFilter
		{
			CONNECTOR(data::Data, data::Data);

			REGISTER(PythonFilter, ()), _connexData(data::INDATA)
			{
				_init = false;
				mPyUser = nullptr;
			}
			data::PyContextData l_pythonContext;

			REGISTER_P(std::string, script_path);
			REGISTER_P(std::string, function_name);
			
			REGISTER_P(std::string, jsonParams);

			std::atomic<bool> _init;

			PyInterpreterState* _m_interp;

			PyThreadState* pyThreadState;
			PyExternalUser* mPyUser;

			//Called from parent thread
			void init_python(const std::string& path);

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

		ADD_CLASS(PythonFilter, script_path, function_name, jsonParams);
	}
}
