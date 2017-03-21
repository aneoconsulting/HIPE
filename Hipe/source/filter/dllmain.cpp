#include <filter/References.h>
#include <filter/tools/RegisterTable.h>
#include <iostream>

#ifndef WIN32
#define ENTRYPOINT __attribute__((constructor))
#endif




ENTRYPOINT void load()
{
	RegisterTable & reg = RegisterTable::getInstance();
	std::cout << "list of types registered : " << std::endl;
	
	for (auto &name : reg.getTypeNames())
	{
		std::cout << name << std::endl;
	}
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

