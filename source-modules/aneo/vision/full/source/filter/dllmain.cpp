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
