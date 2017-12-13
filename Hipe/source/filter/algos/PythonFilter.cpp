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
				Py_Initialize();

			if (!_init.exchange(true))
			{
				std::ostringstream python_path;
				std::ostringstream python2_path;
				python_path << "sys.path.append(r'" << extractDirectoryName(script_path).c_str() << "')";
				std::string workingDir = GetCurrentWorkingDir();
				python2_path << "sys.path.append(r'" << workingDir << "')";
				boost::python::object main = boost::python::import("__main__");
				boost::python::object global(main.attr("__dict__"));
				// Initialize the Python Interpreter
				try
				{
					boost::python::exec("import sys", global, global);
					boost::python::exec(python_path.str().c_str(), global, global);
					boost::python::exec(python2_path.str().c_str(), global, global);
				}
				catch(boost::python::error_already_set & e)
				{
					if (PyErr_Occurred())
						PyErr_Print();
				}
				std::string moduleName = extractFileName(script_path);
				
				
		

			

				/*PyRun_SimpleString("import sys");
				PyRun_SimpleString(python_path.str().c_str());
				PyRun_SimpleString(python2_path.str().c_str());*/
				
			}
		}


		std::string PythonFilter::GetCurrentWorkingDir(void)
		{
			char buff[FILENAME_MAX];
			GetCurrentDir(buff, FILENAME_MAX);
			std::string current_working_dir(buff);
			return current_working_dir;
		}

		HipeStatus process_old()
		{
			PyObject *pName, *pModule, *pDict, *pFunc;
			PyObject *pArgs, *pValue;

			//std::unique_ptr<char[]> py_name(new char[getName().size()]);
			//std::copy(getName().c_str(), getName().c_str() + getName().size(), &py_name[0]);
			//

			//Py_SetProgramName(&py_name[0]);  /* optional but recommended */
			// Build the name object
			//init_python(extractDirectoryName(script_path).c_str());
			std::string moduleName;// = extractFileName(script_path);

			pName = PyString_FromString(moduleName.c_str());

			// Load the module object
			pModule = PyImport_Import(pName);

			if (pModule == NULL)
			{
				if (PyErr_Occurred())
					PyErr_Print();
				Py_DECREF(pName);
				Py_Finalize();
				return UNKOWN_ERROR;
			}
			/* Error checking of pName left out */
			// pDict is a borrowed reference 
			pDict = PyModule_GetDict(pModule);

			if (pDict == NULL)
			{
				Py_DECREF(pName);
				Py_DECREF(pModule);
				Py_Finalize();
				return UNKOWN_ERROR;
			}


			// pFunc is also a borrowed reference 
			pFunc = PyDict_GetItemString(pDict, "process");

			if (pFunc == NULL)
			{
				if (PyErr_Occurred())
					PyErr_Print();

				Py_DECREF(pModule);

				Py_DECREF(pName);

				Py_Finalize();
				return UNKOWN_ERROR;
			}

			if (PyCallable_Check(pFunc))
			{
				pValue = PyObject_CallObject(pFunc, nullptr);
				//boost::python::call<void>(callable, boost::ref(x));
				if (PyErr_Occurred())
					PyErr_Print();
				if (pValue != NULL)
				{
					//printf("Result of call: %ld\n", PyInt_AsLong(pValue));
					Py_DECREF(pValue);
				}


				//Py_DECREF(pDict);
				Py_DECREF(pModule);
				Py_DECREF(pName);


				//Py_Finalize();
			}
			else
			{
				if (PyErr_Occurred())
					PyErr_Print();
				Py_DECREF(pName);
				Py_DECREF(pModule);
				Py_DECREF(pDict);
				Py_Finalize();
				return UNKOWN_ERROR;
			}


			return HipeStatus::OK;
		}

		HipeStatus PythonFilter::process()
		{

			init_python(extractDirectoryName(script_path));
			
			/*PyRun_SimpleString("import sys");
			PyRun_SimpleString(python_path.str().c_str());
			PyRun_SimpleString(python2_path.str().c_str());*/
			
			
			boost::python::object main = boost::python::import("__main__");
			boost::python::object global(main.attr("__dict__"));
			try
			{
				boost::python::object result = boost::python::exec_file(script_path.c_str(), global, global);
				boost::python::object foo = global["process"];
				if (!foo.is_none())
				{
					boost::shared_ptr<pyImageData> o(new pyImageData);

					foo(boost::python::ptr(o.get()));
					pyImageData* py_image_data = o.get();
					std::cout << "Nb rows " << py_image_data->get().rows << " Nb Cols :" << py_image_data->get().cols << std::endl;
				}
			}
			catch (boost::python::error_already_set & e)
			{
				if (PyErr_Occurred())
					PyErr_Print();
				return OK;
			}
			
			return OK;
		}
	}
}
