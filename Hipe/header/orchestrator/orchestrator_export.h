#pragma once


#if defined WIN32 || defined __CYGWIN__
#ifdef ORCHESTRATOR_BUILD
#ifdef __GNUC__
#define ORCHESTRATOR_EXPORT __attribute__ ((dllexport))
#else
#define ORCHESTRATOR_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define ORCHESTRATOR_EXPORT __attribute__ ((dllimport))
#else
#define ORCHESTRATOR_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define ORCHESTRATOR_LOCAL
#else
#if __GNUC__ >= 4
#define ORCHESTRATOR_EXPORT __attribute__ ((visibility ("default")))
#define ORCHESTRATOR_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define ORCHESTRATOR_EXPORT
#define ORCHESTRATOR_LOCAL
#endif
#endif