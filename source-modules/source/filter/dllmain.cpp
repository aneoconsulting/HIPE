//@HIPE_LICENSE@
#if defined(USE_DLIB) && defined(__ALTIVEC__)
	#include <dlib/simd.h>
#endif

#include <filter/References.h>
#include <corefilter/tools/RegisterTable.h>
#include <iostream>
#include <corefilter/filter_export.h>
#include <core/Logger.h>

#include <glog/stl_logging.h>

#include <list>

extern "C"
{
#ifndef WIN32
#define ENTRYPOINT __attribute__((constructor))
#else
#define ENTRYPOINT FILTER_EXPORT
#endif


ENTRYPOINT void load()
{
	RegisterTable & reg = RegisterTable::getInstance();
	
	std::list<std::string> x;

	for (auto &name : reg.getTypeNames())
	{
		x.push_back(name);
		/*std::cout << name << std::endl;*/
	}
	LOG(INFO) << "list of types registered : " << x;
}

#ifdef WIN32
#include <windows.h>

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		load();
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

}