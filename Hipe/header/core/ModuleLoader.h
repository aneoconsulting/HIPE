#pragma once
#include <string>
#include <boost/function_types/components.hpp>
#include <boost/function_types/function_pointer.hpp>
#include <core/misc.h>

namespace core
{
#ifdef WIN32
#include <windows.h>

	class ModuleLoader
	{
		std::string _filename;
		HINSTANCE dllHandle;
	public:
		ModuleLoader(std::string path) : _filename(path), dllHandle(nullptr)
		{
		}

		//Define the function prototype
		typedef short (CALLBACK* FindArtistType)(LPCTSTR);

		void loadLibrary()
		{
			//Get the directory name
			std::string dll_dir = extractDirectoryName(_filename);

			addEnv(dll_dir);
			
			//Load the dll and keep the handle to it
			dllHandle = LoadLibrary(_filename.c_str());
			if (!dllHandle)
			{
				std::stringstream msg;
				msg << "Could not locate the shared library \"" << _filename + "\"";
				std::cerr << msg.str() << std::endl;
				throw HipeException(msg.str());
			}

			
		}

		template <typename T>
		std::function<T> callFunction(const std::string& funcName)
		{
			// Locate function in DLL.
			FARPROC lpfnGetProcessID = GetProcAddress(dllHandle, funcName.c_str());

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
			BOOL freeResult = FALSE;
			//Free the library:
			freeResult = FreeLibrary(dllHandle);
		}
	};
#endif // WIN32
}
