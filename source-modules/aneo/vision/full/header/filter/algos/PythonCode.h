//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
