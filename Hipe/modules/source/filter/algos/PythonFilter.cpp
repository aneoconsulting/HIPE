#include <filter/algos/PythonFilter.h>
#include <core/HipeStatus.h>


extern "C"
{
#include <Python.h>
}

// #define WINDOWS  /* uncomment this line to use it for windows.*/ 
#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include<iostream>

#include <boost/python.hpp>
#include <pydata/pyImageData.h>

namespace filter
{
	namespace algos
	{
		void PythonFilter::init_python(const std::string& path)
		{
			if (!Py_IsInitialized())
			{
				Py_Initialize();
			}
			if (!_init.exchange(true))
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
					}
					catch (boost::python::error_already_set& e)
					{
						if (PyErr_Occurred())
							PyErr_Print();
					}
					l_pythonContext.This().script = boost::python::import(moduleName.c_str());
					l_pythonContext.This().global = l_pythonContext.This().script.attr("__dict__");
					
					boost::python::object result = boost::python::exec_file(script_path.c_str(), l_pythonContext.This().global);
				}
				
			}
		}


		std::string PythonFilter::GetCurrentWorkingDir(void)
		{
			char buff[FILENAME_MAX];
			GetCurrentDir(buff, FILENAME_MAX);
			std::string current_working_dir(buff);
			return current_working_dir;
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

			init_python(extractDirectoryName(script_path));


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

					//foo(boost::python::ptr(o.get()));
					boost::python::object result = foo(boost::python::ptr(o.get()));
					
					pyImageData* py_image_data = o.get();
					std::cout << "Nb rows " << py_image_data->get().rows << " Nb Cols :" << py_image_data->get().cols << std::endl;
				}
			}
			catch (boost::python::error_already_set& e)
			{
				if (PyErr_Occurred())
					PyErr_Print();
				return OK;
			}
			_connexData.push(l_pythonContext);

			return OK;
		}
	}
}
