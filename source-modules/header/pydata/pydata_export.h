//@HIPE_LICENSE@
#pragma once


#pragma once

#pragma once


#if defined _WIN32 || defined __CYGWIN__
#ifdef PYDATA_BUILD
#ifdef __GNUC__
#define PYDATA_EXPORT __attribute__ ((dllexport))
#else
#define PYDATA_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define PYDATA_EXPORT __attribute__ ((dllimport))
#else
#define PYDATA_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define PYDATA_LOCAL
#else
#if __GNUC__ >= 4
#define PYDATA_EXPORT __attribute__ ((visibility ("default")))
#define PYDATA_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define PYDATA_EXPORT
#define PYDATA_LOCAL
#endif
#endif