#pragma once

#if defined WIN32 || defined __CYGWIN__
	#ifdef JSON_BUILD
		#ifdef __GNUC__
			#define JSON_EXPORT __attribute__ ((dllexport))
		#else
			#define JSON_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
			#define JSON_EXTERN extern
		#endif
	#else
		#ifdef __GNUC__
			#define JSON_EXPORT __attribute__ ((dllimport))
			#define JSON_EXTERN extern
		#else
			#define JSON_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
			#define JSON_EXTERN 
		#endif
	#endif

	#define JSON_LOCAL

#else
	#if __GNUC__ >= 4
		#define JSON_EXPORT __attribute__ ((visibility ("default")))
		#define JSON_LOCAL  __attribute__ ((visibility ("hidden")))
		#define JSON_EXTERN 
	#else
		#define JSON_EXPORT
		#define JSON_LOCAL
		#define JSON_EXTERN 
	#endif
#endif
