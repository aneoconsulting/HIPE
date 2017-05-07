#pragma once


#if defined _WIN32 || defined __CYGWIN__
#ifdef CORE_BUILD
#ifdef __GNUC__
#define CORE_EXPORT __attribute__ ((dllexport))
#else
#define CORE_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define CORE_EXPORT __attribute__ ((dllimport))
#else
#define CORE_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define CORE_LOCAL
#else
#if __GNUC__ >= 4
#define CORE_EXPORT __attribute__ ((visibility ("default")))
#define CORE_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define CORE_EXPORT
#define CORE_LOCAL
#endif
#endif