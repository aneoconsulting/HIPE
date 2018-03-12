#pragma once
#include <string>
#include <boost/function_types/components.hpp>
#include <boost/function_types/function_pointer.hpp>
#include <core/misc.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace core
{

	class ModuleLoader
	{
		std::string _filename;
#ifdef WIN32
		HINSTANCE dllHandle;
#else
		void * dllHandle;
#endif

	public:
		ModuleLoader(std::string path) : _filename(path), dllHandle(nullptr)
		{
		}

		//Define the function prototype
		void loadLibrary()
		{
			//Get the directory name
			std::string dll_dir = extractDirectoryName(_filename);

			addEnv(dll_dir);

#ifdef WIN32

			//Load the dll and keep the handle to it
			dllHandle = LoadLibrary(_filename.c_str());
#else
			//#FIXME
			dllHandle = dlopen(_filename.c_str(), RTLD_LAZY);
#endif
			
			if (!dllHandle)
			{
				std::stringstream msg;
				msg << "Could not locate the shared library \"" << _filename + "\"";
#ifdef WIN32
				std::cerr << msg.str() << std::endl;
#else
				std::cerr << msg.str() << " DL message : " << dlerror() << std::endl;

#endif
				throw HipeException(msg.str());
			}
			
		}


		template <typename T>
		std::function<T> callFunction(const std::string& funcName)
		{

			// Locate function in DLL.
#ifdef WIN32
			
			FARPROC lpfnGetProcessID = GetProcAddress(dllHandle, funcName.c_str());
#else
			void * lpfnGetProcessID = dlsym(dllHandle, funcName.c_str());
#endif
			// Check if function was located.
			if (!lpfnGetProcessID)
			{
				std::stringstream msg;
				msg << "Could not locate the function \"" << funcName << "\" in DLL\"" << _filename + "\"";
				std::cerr << msg.str() << std::endl;	
				throw HipeException(msg.str());
			}
			namespace ft = boost::function_types;
			typedef typename ft::function_pointer< typename ft::components<T>::type >::type fp_t;
			fp_t fun_ptr;

			*reinterpret_cast<void**>(&fun_ptr) = lpfnGetProcessID;
			// Create function object from function pointer.
			//std::function<T> func(reinterpret_cast<T *> (lpfnGetProcessID));

			return fun_ptr;
		}

		
		void freeLibrary()
		{
#ifdef WIN32
			
			BOOL freeResult = FALSE;
			//Free the library:
			freeResult = FreeLibrary(dllHandle);
#else
			//FIXME
#endif
		}
	};

}
