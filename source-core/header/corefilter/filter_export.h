//@HIPE_LICENSE@
#pragma once

#pragma once


#if defined _WIN32 || defined __CYGWIN__
	#ifdef FILTER_BUILD
		#ifdef __GNUC__
			#define FILTER_EXPORT __attribute__ ((dllexport))
		#else
			#define FILTER_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
			#define FILTER_EXTERN extern
		#endif
	#else
		#ifdef __GNUC__
			#define FILTER_EXPORT __attribute__ ((dllimport))
			#define FILTER_EXTERN extern
		#else
			#define FILTER_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
			#define FILTER_EXTERN 
		#endif
	#endif
	#define FILTER_LOCAL
#else
	#if __GNUC__ >= 4
		#define FILTER_EXPORT __attribute__ ((visibility ("default")))
		#define FILTER_LOCAL  __attribute__ ((visibility ("hidden")))
		#define FILTER_EXTERN extern
	#else
		#define FILTER_EXPORT
		#define FILTER_LOCAL
		#define FILTER_EXTERN 
	#endif
#endif