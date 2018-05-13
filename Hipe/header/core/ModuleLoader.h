//@HIPE_LICENSE@
#pragma once
#include <string>
#include <core/misc.h>
#include <core/StringTools.h>
#include <core/HipeException.h>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "ImageHlp.h"
#else
#include <dlfcn.h>
#endif

#pragma warning(push, 0)
#include <boost/function_types/components.hpp>
#include <boost/function_types/function_pointer.hpp>
#pragma warning(pop)


namespace core
{

	class ModuleLoader
	{
		std::string _filename;
#ifdef WIN32
		HINSTANCE dllHandle;

		template <class T>
		PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, T* pNTHeader) // 'T' == PIMAGE_NT_HEADERS 
		{
			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
			unsigned i;

			for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++)
			{
				// This 3 line idiocy is because Watcom's linker actually sets the
				// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
				DWORD size = section->Misc.VirtualSize;
				if (0 == size)
					size = section->SizeOfRawData;

				// Is the RVA within this section?
				if ((rva >= section->VirtualAddress) &&
					(rva < (section->VirtualAddress + size)))
					return section;
			}

			return 0;
		}

		template <class T>
		LPVOID GetPtrFromRVA(DWORD rva, T* pNTHeader, PBYTE imageBase) // 'T' = PIMAGE_NT_HEADERS 
		{
			PIMAGE_SECTION_HEADER pSectionHdr;
			INT delta;

			pSectionHdr = GetEnclosingSectionHeader(rva, pNTHeader);
			if (!pSectionHdr)
				return 0;

			delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
			return (PVOID)(imageBase + rva - delta);
		}


		void DumpDllFromPath(const wchar_t* path, std::vector<std::string> & stackDeps)
		{
			char name[300];
			wcstombs(name, path, 300);

			PLOADED_IMAGE image = ImageLoad(name, 0);

			if (! image )
			{
				std::cout <<  " Deps dllname = " << name << " [ IMAGE NOT FOUND ]" << std::endl;
				stackDeps.push_back(name);
				std::sort(stackDeps.begin(), stackDeps.end());
				return;
			}
			std::vector<std::string> listDeps;

			if (image->FileHeader->OptionalHeader.NumberOfRvaAndSizes >= 2)
			{
				PIMAGE_IMPORT_DESCRIPTOR importDesc =
					(PIMAGE_IMPORT_DESCRIPTOR)GetPtrFromRVA(
						image->FileHeader->OptionalHeader.DataDirectory[1].VirtualAddress,
						image->FileHeader, image->MappedAddress);
				if (importDesc)
				{
					while (1)
					{
						// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
						if ((importDesc->TimeDateStamp == 0) && (importDesc->Name == 0))
							break;

						//printf("  %s\n", );
						listDeps.push_back((char *)GetPtrFromRVA(importDesc->Name,
													   image->FileHeader,
													   image->MappedAddress));
						importDesc++;
					}
				}
				else
				{
					std::cout <<  " Deps dllname = " << image->ModuleName << " [ Warnings : MISSING IMPORT ]" << std::endl;
				}


			}

			for (std::string dllname : listDeps)
			{
				//std::cout <<  " Deps dllname = " << dllname;
				HINSTANCE dllDepsHandle = LoadLibraryEx(_filename.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
				if (dllDepsHandle)
				{
					//std::cout << " [ OK ] " << std::endl;
					FreeLibrary(dllDepsHandle);
					std::wstring widestr = std::wstring(dllname.begin(), dllname.end());
					if (std::find(stackDeps.begin(), stackDeps.end(), dllname) == stackDeps.end())
					{
						stackDeps.push_back(dllname);
						std::sort(stackDeps.begin(), stackDeps.end());
						DumpDllFromPath(widestr.c_str(), stackDeps);
					}
				}
				else
				{
					std::cout <<  " Deps dllname = " << dllname << " [ NOT FOUND ] " << std::endl;
				}
			}

			ImageUnload(image);
		}

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
			DWORD dw = GetLastError();
			
#else
			//#FIXME
			dllHandle = dlopen(_filename.c_str(), RTLD_LAZY);
#endif
			
			if (!dllHandle)
			{
				std::stringstream msg;
				msg << "Could not locate the shared library \"" << _filename + "\"";
#ifdef WIN32
				wchar_t err[4096];
				memset(err, 0, 4096);
				std::wstring widestr = std::wstring(_filename.begin(), _filename.end());
				std::vector<std::string> stackDeps;
				DumpDllFromPath(widestr.c_str(), stackDeps);
				FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw,
							  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 4095, NULL);
				
				std::cerr << msg.str() << " Last Error " << err << std::endl;
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
