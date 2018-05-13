//@HIPE_LICENSE@
#pragma once
#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
struct hipetimeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};
#else
#include <sys/time.h>
typedef struct timeval hipetimeval;
#endif

#include <string>

void hipe_usleep(long long usec);
int hipe_gettimeofday(hipetimeval * p, void* tz);

bool isFileExist(std::string filename);

bool isDirExist(const std::string & dirname);

bool createDirectory(const std::string & dirpath);

std::string PathSeparator();

void addEnv(std::string path);

std::string GetCurrentWorkingDir(void);
