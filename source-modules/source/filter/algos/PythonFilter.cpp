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

#include <filter/algos/PythonFilter.h>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <core/python/pyThreadSupport.h>


extern "C"
{
#include <Python.h>
}

// #define WINDOWS  /* uncomment this line to use it for windows.*/ 

#include<iostream>

#pragma warning(push, 0)
#include <boost/python.hpp>
#pragma warning(pop)

#include <pydata/pyImageData.h>

namespace filter
{
	namespace algos
	{
		class Hipe_PyEnsureGIL
		{
		public:
			Hipe_PyEnsureGIL() :
				_state(PyGILState_Ensure())
			{
			}

			~Hipe_PyEnsureGIL()
			{
				PyGILState_Release(_state);
			}

		private:
			PyGILState_STATE _state;
		};

		class Hipe_PyAllowThreads
		{
		public:
			Hipe_PyAllowThreads() :
				_state(PyEval_SaveThread())
			{
			}

			~Hipe_PyAllowThreads()
			{
				PyEval_RestoreThread(_state);
			}

		private:
			PyThreadState* _state;
		};

		void PythonFilter::init_python(const std::string& path)
		{
			{
				std::ostringstream python_path;
				std::ostringstream python2_path;
				python_path << "sys.path.append(r'" << extractDirectoryName(script_path).c_str() << "')";
				std::string workingDir = GetCurrentWorkingDir();
				python2_path << "sys.path.append(r'" << workingDir << "')";


				

				if (l_pythonContext.This().main.is_none())
				{
					l_pythonContext.This().main = boost::python::import("__main__");

					std::string moduleName = extractFileName(script_path);

					l_pythonContext.This().global = l_pythonContext.This().main.attr("__dict__");
					l_pythonContext.This().local = l_pythonContext.This().main.attr("__dict__");

					// Initialize the Python Interpreter
					try
					{
						boost::python::exec("import sys", l_pythonContext.This().global, l_pythonContext.This().local);
						boost::python::exec(python_path.str().c_str(), l_pythonContext.This().global, l_pythonContext.This().local);
						boost::python::exec(python2_path.str().c_str(), l_pythonContext.This().global, l_pythonContext.This().local);

						l_pythonContext.This().script = boost::python::import(moduleName.c_str());
						boost::python::import("__main__").attr("__dict__")[moduleName.c_str()] = l_pythonContext.This().script;

						
						//Reload module when the python has already loaded the module
						

						std::stringstream reload_command;
						reload_command  << "reload(" << moduleName.c_str() << ")\n";
						
						
						boost::python::exec(reload_command.str().c_str(), l_pythonContext.This().global, l_pythonContext.This().local);
						
						l_pythonContext.This().global = l_pythonContext.This().script.attr("__dict__");
					}
					catch (boost::python::error_already_set& e)
					{
						if (PyErr_Occurred())
							PyErr_Print();
					}
				}
			}
		}


		HipeStatus PythonFilter::process()
		{
			std::vector<data::Data> input;

			while (_connexData.size() != 0)
			{
				data::Data d_ctx = _connexData.pop();
				if (d_ctx.getType() == data::IODataType::PYCTX)
				{
					l_pythonContext = static_cast<const data::PyContextData&>(d_ctx);
				}
				else
				{
					input.push_back(d_ctx);
				}
			}

			PyExternalUser::Use use(*mPyUser);
			try
			{
				std::string l_function_name = "process";
				if (!function_name.empty())
				{
					l_function_name = function_name;
				}


				boost::python::object foo = l_pythonContext.This().global[l_function_name.c_str()];

				if (!foo.is_none())
				{
					boost::shared_ptr<pyImageData> o(new pyImageData);
					data::ImageData img = input[0];
					o->assign(img.getMat());
					//foo(boost::python::ptr(o.get()));

					boost::python::object result;
					
					result = foo(boost::python::ptr(o.get()));
					

					pyImageData* py_image_data = o.get();
					//std::cout << "Nb rows " << py_image_data->get().rows << " Nb Cols :" << py_image_data->get().cols << std::endl;
					data::ImageData res_process(py_image_data->get());

					PUSH_DATA(res_process);
				}
			}
			catch (boost::python::error_already_set& e)
			{
				if (PyErr_Occurred())
					PyErr_Print();
				return OK;
			}
			PUSH_DATA(l_pythonContext);

			return OK;
		}

		void PythonFilter::dispose()
		{
			if (_init.exchange(false))
			{
				mPyUser = nullptr;
			}
		}

		void PythonFilter::onLoad(void* interp)
		{
			if (!mPyUser)
			{
				_m_interp = static_cast<PyInterpreterState*>(interp);
				
			}

			init_python(extractDirectoryName(script_path));
			
		}

		void PythonFilter::onStart(void* pyThreadState)
		{
			
			mPyUser = static_cast<PyExternalUser*>(pyThreadState);
		}

	}
}
