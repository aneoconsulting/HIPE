#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include <string>

void hipe_usleep(long long usec);
int hipe_gettimeofday(struct timeval* p, void* tz);

bool isFileExist(std::string filename);
