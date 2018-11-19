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


enum ImageFileType
{
	IMAGE_FILE_JPG, // joint photographic experts group - .jpeg or .jpg
	IMAGE_FILE_PNG, // portable network graphics
	IMAGE_FILE_GIF, // graphics interchange format 
	IMAGE_FILE_TIFF, // tagged image file format
	IMAGE_FILE_BMP, // Microsoft bitmap format
	IMAGE_FILE_WEBP, // Google WebP format, a type of .riff file
	IMAGE_FILE_ICO, // Microsoft icon format
	IMAGE_FILE_INVALID, // unidentified image types.
};

ImageFileType getImageTypeByMagic(const unsigned char* data, unsigned int len);

std::string getExtensionFromImageData(const unsigned char* data, unsigned int len);

void hipe_usleep(long long usec);
int hipe_gettimeofday(hipetimeval * p, void* tz);

bool isFileExist(std::string filename, bool needThrow = true);

bool isDirExist(const std::string & dirname);

bool createDirectory(const std::string & dirpath);

std::string PathSeparator();

std::string getEnv(std::string path);

void addEnv(std::string path);

void addVarEnv(std::string key, std::string value);

std::string GetCurrentWorkingDir(void);

bool SetCurrentWorkingDir(const std::string & path);
