//@HIPE_LICENSE@
#include <core/misc.h>
#include <string>

#include <sstream>

#pragma warning(push, 0)
#include <boost/filesystem/operations.hpp>
#pragma warning(pop)

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

void hipe_usleep(long long usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}


int hipe_gettimeofday(hipetimeval* p, void* tz)
{
	ULARGE_INTEGER ul; // As specified on MSDN.
	FILETIME ft;

	// Returns a 64-bit value representing the number of
	// 100-nanosecond intervals since January 1, 1601 (UTC).
	GetSystemTimeAsFileTime(&ft);

	// Fill ULARGE_INTEGER low and high parts.
	ul.LowPart = ft.dwLowDateTime;
	ul.HighPart = ft.dwHighDateTime;
	// Convert to microseconds.
	ul.QuadPart /= 10ULL;
	// Remove Windows to UNIX Epoch delta.
	ul.QuadPart -= 11644473600000000ULL;
	// Modulo to retrieve the microseconds.
	p->tv_usec = (long)(ul.QuadPart % 1000000LL);
	// Divide to retrieve the seconds.
	p->tv_sec = (long)(ul.QuadPart / 1000000LL);

	return 0;
}

#include <cstdlib>
#include <stdlib.h>
#include <sstream>
void addEnv(std::string path)
{
	char env_p[163840];
	GetEnvironmentVariable("PATH", env_p, 163840);
	std::stringstream new_path;
	new_path << path << ";" << env_p;
	std::string cs = new_path.str();
	SetEnvironmentVariable("PATH", cs.c_str());

	GetEnvironmentVariable("PATH", env_p, 163840);


}

void addVarEnv(std::string key, std::string value)
{
	char env_p[163840];
	GetEnvironmentVariable(key.c_str(), env_p, 163840);
	std::stringstream new_path;
	new_path << value << ";" << env_p;
	std::string cs = new_path.str();
	SetEnvironmentVariable(key.c_str(), cs.c_str());

	GetEnvironmentVariable(key.c_str(), env_p, 163840);


}

#else
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

int hipe_gettimeofday(hipetimeval* p, void* tz) {
	return gettimeofday(p, (struct timezone *)tz);
}

void hipe_usleep(long long usec)
{
	usleep(usec);
}

void addEnv(std::string path)
{
	char * env_p;
	env_p = getenv("PATH");
	std::stringstream new_path;
	new_path << path << ";" << env_p;
	std::string cs = new_path.str();
	setenv("PATH", cs.c_str(), 1);
	
}

void addVarEnv(std::string key, std::string value)
{
	char * env_p;
	env_p = getenv(key.c_str());
	std::stringstream new_path;
	new_path << value << ";" << env_p;
	std::string cs = new_path.str();
	setenv(key.c_str(), cs.c_str(), 1);

}

#endif

bool isFileExist(std::string filename)
{
	if (!(boost::filesystem::exists(boost::filesystem::path(filename)) && boost::filesystem::is_regular_file(boost::filesystem::path(filename))))
	{
		std::stringstream buildMsg;
		buildMsg << "file [";
		buildMsg << filename;
		buildMsg << "] not found";
		throw std::invalid_argument(buildMsg.str());
	}
	return true;
}

bool isDirExist(const std::string & dirname)
{
	if (!(boost::filesystem::exists(boost::filesystem::path(dirname)) && boost::filesystem::is_directory(boost::filesystem::path(dirname))))
	{
		return false;
	}
	return true;

}

bool createDirectory(const std::string & dirpath)
{
	if (isDirExist(dirpath)) return false;

	return boost::filesystem::create_directory(boost::filesystem::path(dirpath));
}

#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string GetCurrentWorkingDir(void)
{
	char buff[BUFSIZ];
	GetCurrentDir(buff, BUFSIZ);
	std::string current_working_dir(buff);
	return current_working_dir;
}

std::string PathSeparator()
{
#if defined _WIN32 || defined __CYGWIN__
		return std::string("\\");
#else
		return std::string("/");
#endif
}