//@HIPE_LICENSE@
#pragma once

#pragma once

#pragma once


#if defined _WIN32 || defined __CYGWIN__
	#ifdef DATA_BUILD
		#ifdef __GNUC__
			#define DATA_EXPORT __attribute__ ((dllexport))
		#else
			#define DATA_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
		#endif
	#else
		#ifdef __GNUC__
			#define DATA_EXPORT __attribute__ ((dllimport))
		#else
			#define DATA_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
		#endif
	#endif

	#define DATA_LOCAL

#else
	#if __GNUC__ >= 4
		#define DATA_EXPORT __attribute__ ((visibility ("default")))
		#define DATA_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define DATA_EXPORT
		#define DATA_LOCAL
	#endif
#endif
