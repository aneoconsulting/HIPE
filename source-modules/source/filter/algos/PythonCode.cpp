//@HIPE_LICENSE@
#include <boost/algorithm/string.hpp>

#include <filter/algos/PythonCode.h>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <core/python/pyThreadSupport.h>
#include <corefilter/tools/Localenv.h>
#include "data/FileVideoInput.h"
#include <coredata/Data.h>
#include "pydata/pyShapeData.h"
#include "data/ShapeData.h"
#include <glog/logging.h>

#include <boost/circular_buffer.hpp>


extern "C" {
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
		// decode a Python exception into a string
		std::string pythonCode_pyerror()
		{
			using namespace boost::python;
			using namespace boost;

			PyObject *exc, *val, *tb;
			object formatted_list, formatted;
			PyErr_Fetch(&exc, &val, &tb);
			PyErr_NormalizeException(&exc, &val, &tb);
			handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
			object traceback(import("traceback"));
			if (!tb)
			{
				object format_exception_only(traceback.attr("format_exception_only"));
				formatted_list = format_exception_only(hexc, hval);
			}
			else
			{
				object format_exception(traceback.attr("format_exception"));
				formatted_list = format_exception(hexc, hval, htb);
			}
			formatted = str("\n").join(formatted_list);
			return extract<std::string>(formatted);
		}

		class PythonStdIoRedirect
		{
		public:
			typedef boost::circular_buffer<std::string> ContainerType;

			void Write(std::string const& str)
			{
				if (m_outputs.capacity() < 100)
					m_outputs.resize(100);
				m_outputs.push_back(str);
			}

			static std::string GetOutput()
			{
				std::string ret;
				std::stringstream ss;
				for (boost::circular_buffer<std::string>::const_iterator it = m_outputs.begin();
				     it != m_outputs.end();
				     it++)
				{
					ss << *it;
				}
				m_outputs.clear();
				ret = ss.str();
				return ret;
			}

		private:
			static ContainerType m_outputs; // must be static, otherwise output is missing
		};

		PythonStdIoRedirect::ContainerType PythonStdIoRedirect::m_outputs;


		void PythonCode::add_python_path(const std::string& python_path)
		{
			std::vector<std::string> results;

			boost::split(results, python_path, [](char c) { return c == ';'; });
			
			for (auto p : results)
			{
				std::ostringstream python_path_builder;
				python_path_builder << "sys.path.insert(0, r'" << p << "'); os.environ['PYTHONPATH'] = r'" << p <<"' + os.pathsep + os.environ['PATH']";

				boost::python::exec(python_path_builder.str().c_str(), l_pythonContext.This().global,
				                    l_pythonContext.This().local);

				LOG(INFO) << "Add Python path : " << p.c_str() << std::endl;
			}
		}

		void PythonCode::initialize_python_paths()
		{
			boost::python::exec("import sys", l_pythonContext.This().global, l_pythonContext.This().local);
			boost::python::exec("import imp", l_pythonContext.This().global, l_pythonContext.This().local);
			boost::python::exec("import os", l_pythonContext.This().global, l_pythonContext.This().local);
			add_python_path(corefilter::getLocalEnv().getValue("python_dll_path"));

			std::string workingDir = GetCurrentWorkingDir();

			add_python_path(workingDir);
			add_python_path(workingDir + "/" + extractDirectoryName(temp_script_file).c_str());

			std::stringstream new_workingDir_command;
			std::string new_workingDir = extractDirectoryName(temp_script_file);
			add_python_path(new_workingDir);

			add_python_path(corefilter::getLocalEnv().getValue("pydata_path"));

			
		}

		void PythonCode::init_python(const std::string& path)
		{
			if (l_pythonContext.This().main.is_none())
			{
				l_pythonContext.This().main = boost::python::import("__main__");

				std::string moduleName = extractFileName(temp_script_file);

				l_pythonContext.This().global = l_pythonContext.This().main.attr("__dict__");
				l_pythonContext.This().local = l_pythonContext.This().main.attr("__dict__");

				// Initialize the Python Interpreter
				try
				{
					initialize_python_paths();

					//Write file with code
					 std::ofstream myfile;
					myfile.open (temp_script_file);
					myfile << code_impl << "\n";
					myfile.close();

					l_pythonContext.This().script = boost::python::import(moduleName.c_str());
					l_pythonContext.This().main.attr("__dict__")[moduleName.c_str()] = l_pythonContext
					                                                                         .This().script;
					//Redirect Stdio from pyhton
					l_pythonContext.This().main.attr("__dict__")["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
					.def("write", &PythonStdIoRedirect::Write);
					
					PythonStdIoRedirect python_stdio_redirector;
					object kwds_proxy = boost::python::import("sys");
					//kwds_proxy.attr("stderr") = python_stdio_redirector;
					kwds_proxy.attr("stdout") = python_stdio_redirector;

					//Reload module when the python has already loaded the module


					std::stringstream reload_command;
					reload_command << "imp.reload(" << moduleName.c_str() << ");\n";


					boost::python::exec(reload_command.str().c_str(), l_pythonContext.This().global,
					                    l_pythonContext.This().local);


					l_pythonContext.This().global = l_pythonContext.This().script.attr("__dict__");

					//Is there any function name init_process ??
					std::stringstream init_func_name;
					init_func_name << "init_" << function_name;
					boost::python::object pythonInitCall;

					try {
						pythonInitCall = l_pythonContext.This().global[init_func_name.str()];
						boost::python::object result;

						result = pythonInitCall();
					}
					catch (boost::python::error_already_set& e)
					{
					}
					
				}
				catch (boost::python::error_already_set& e)
				{
					if (PyErr_Occurred())
					{
						std::string msg = pythonCode_pyerror();
						LOG(ERROR) << msg << std::endl;
						throw HipeException("Python error : " + msg);
					}
				}
			}
		}

		void PythonCode::push_result(const boost::python::object& object)
		{
			if (object.is_none())
				return;

			if (boost::python::extract<cv::Mat>(object).check())
			{
				cv::Mat res = boost::python::extract<cv::Mat>(object);

				data::ImageData res_process(res);

				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<std::tuple<int, int>>(object).check())
			{
				std::tuple<int, int> res = boost::python::extract<std::tuple<int, int>>(object);
				std::vector<cv::Point> vec;

				vec.push_back(cv::Point(std::get<0>(res), std::get<1>(res)));

				data::ShapeData res_process;

				res_process.add(vec);

				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<std::tuple<float, float>>(object).check())
			{
				std::tuple<float, float> res = boost::python::extract<std::tuple<float, float>>(object);
				std::vector<cv::Point> vec;

				vec.push_back(cv::Point(std::get<0>(res), std::get<1>(res)));

				data::ShapeData res_process;

				res_process.add(vec);

				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<std::tuple<int, int, int, int>>(object).check())
			{
				std::tuple<int, int, int, int> res = boost::python::extract<std::tuple<int, int, int, int>>(object);
				cv::Rect rect = cv::Rect(std::get<0>(res), std::get<1>(res), std::get<3>(res), std::get<3>(res));

				data::ShapeData res_process;

				res_process.add(rect);

				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<pyShapeData>(object).check())
			{
				data::ShapeData res_process;
				pyShapeData data = boost::python::extract<pyShapeData>(object);

				//std::copy (b.begin(), b.end(), std::back_inserter(a));
				for (cv::Rect rect : data.rects_array())
				{
					res_process.RectsArray().push_back(rect);
				}

				for (std::vector<cv::Point2f> quad : data.quadrilatere())
				{
					res_process.QuadrilatereArray().push_back(quad);
				}

				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<std::string>(object).check())
			{
				std::string res = boost::python::extract<std::string>(object);
				data::ShapeData res_process;
				res_process.IdsArray().push_back(res);
				res_process.RectsArray().push_back(cv::Rect(0, 0, 1, 1));
				PUSH_DATA(res_process);
			}
			else if (boost::python::extract<boost::python::list>(object).check())
			{
				boost::python::list ns = boost::python::extract<boost::python::list>(object);

				data::ShapeData res_process;
				/*for (int i = 0; i < len(ns); ++i)
				{
					
				}
			 
			 res_process.IdsArray().push_back(res);
			 res_process.RectsArray().push_back(cv::Rect(0, 0, 1, 1));
			 PUSH_DATA(res_process);*/
				throw HipeException("The returned list from python isn't yet implemented");
			}
			else
			{
				std::string type_name = boost::python::extract<std::string>(object.attr("__name__"))();
				
				LOG(WARNING) << std::string("The type ") + type_name + "isn't yet managed in HIPE as result of function call";
				data::ShapeData shape;
				PUSH_DATA(shape);
			}
		}


		HipeStatus PythonCode::process()
		{
			std::vector<data::Data> input;

			if (_connexData.empty()) return OK;

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


				boost::python::object pythonFunctionCall = l_pythonContext.This().global[l_function_name.c_str()];

				if (!pythonFunctionCall.is_none())
				{
					boost::shared_ptr< pyImageData > o(new pyImageData);
					cv::Mat inMat;
					if (!input.empty())
					{
						data::ImageData img = input[0];
						inMat = img.getMat();
						o->assign(inMat);
					}

					{
						boost::python::object result;
						PythonStdIoRedirect python_stdio_redirector;

						result = pythonFunctionCall(boost::python::ptr(o.get()));
						std::string captured_python_output = python_stdio_redirector.GetOutput();
						LOG(INFO) << "Python call : [" << captured_python_output << "]" << std::endl;

						push_result(result);
					}
				}

			//Transfer python context to PythonFilter group
			for (auto& pairs : this->getChildrens())
			{
				if (pairs.second->isPython())
				{
					data::DataPort& data_port = static_cast<data::DataPort &>(pairs.second->getConnector().getPort());
					data_port.push(l_pythonContext);
				}
			}

			}
			catch (boost::python::error_already_set& e)
			{
				if (PyErr_Occurred())
				{
					std::string msg = pythonCode_pyerror();
					LOG(ERROR) << msg << std::endl;
					throw HipeException("Python error : " + msg);
				}
				return END_OF_STREAM;
			}

			//Transfer python context to PythonCode group
			for (auto& pairs : this->getChildrens())
			{
				if (pairs.second->isPython())
				{
					data::DataPort& data_port = static_cast<data::DataPort &>(pairs.second->getConnector().getPort());
					data_port.push(l_pythonContext);
				}
			}


			return OK;
		}

		void PythonCode::dispose()
		{
			if (_init.exchange(false))
			{
				mPyUser = nullptr;
			}
		}

		void PythonCode::onLoad(void* interp)
		{
			if (!mPyUser)
			{
				_m_interp = static_cast<PyInterpreterState*>(interp);
			}

			init_python(extractDirectoryName(temp_script_file));
		}

		void PythonCode::onStart(void* pyThreadState)
		{
			mPyUser = static_cast<PyExternalUser*>(pyThreadState);
		}
	}
}
