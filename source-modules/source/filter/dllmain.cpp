//@HIPE_LICENSE@
#if defined(USE_DLIB) && defined(__ALTIVEC__)
	#include <dlib/simd.h>
#endif
typedef struct _GError GError;

struct _GError
{
  unsigned int       domain;
  int         code;
  char       *message;
};

#ifndef g_filename_to_uri
#include <malloc.h>
#include <string.h>
//extern char *g_filename_to_uri_utf8   (const char *filename, const char *hostname, GError     **error) ;
char *g_filename_to_uri   (const char *filename, const char *hostname, GError     **error)
{
	/*char *copy = (char *)(malloc(strlen(filename)));
	strcpy(copy, filename);*/
	return strdup(filename);
}
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


int WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    unsigned long fdwReason,     // reason for calling function
    void* lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
	case 1:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		load();
		break;

	case 2:
		// Do thread-specific initialization.
		break;

	case 3:
		// Do thread-specific cleanup.
		break;

	case 0:
		// Perform any necessary cleanup.
		break;
    }
    return 1;  // Successful DLL_PROCESS_ATTACH.
}
#endif

}