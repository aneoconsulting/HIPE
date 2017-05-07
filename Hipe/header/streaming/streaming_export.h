#pragma once


#if defined _WIN32 || defined __CYGWIN__
#ifdef STREAMING_BUILD
#ifdef __GNUC__
#define STREAMING_EXPORT __attribute__ ((dllexport))
#else
#define STREAMING_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define STREAMING_EXPORT __attribute__ ((dllimport))
#else
#define STREAMING_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define STREAMING_LOCAL
#else
#if __GNUC__ >= 4
#define STREAMING_EXPORT __attribute__ ((visibility ("default")))
#define STREAMING_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define STREAMING_EXPORT
#define STREAMING_LOCAL
#endif
#endif