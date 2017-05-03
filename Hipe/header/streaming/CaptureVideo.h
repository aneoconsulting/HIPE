#pragma once
#include <string>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <core/HipeException.h>
#include <map>
#include <mutex>
#include <core/Singleton.h>

namespace std {
	class mutex;
}

namespace cv {
	class Mat;
}

class CaptureVideo
{
protected:

	const std::string _path;

public:
	CaptureVideo(const std::string & path) : _path(path) {}

	virtual HipeStatus open() { throw HipeException("Cannot implement Interface Capture video"); };
	virtual HipeStatus close() { throw HipeException("Cannot implement Interface Capture video"); };
	virtual HipeStatus destroy() { throw HipeException("Cannot implement Interface Capture video"); };
	virtual HipeStatus create() { throw HipeException("Cannot implement Interface Capture video"); };
	virtual HipeStatus read(cv::Mat &image) { throw HipeException("Cannot implement Interface Capture video"); };
};

class DLL_PUBLIC CaptureVideoFactory;
template class DLL_PUBLIC Singleton<CaptureVideoFactory>;

class DLL_PUBLIC CaptureVideoFactory : public Singleton<CaptureVideoFactory>
{
protected:
	std::map<std::string, CaptureVideo *> _captureVideoTable;
	std::mutex locking;
public:
	static CaptureVideo* getCaptureVideo(std::string& captureVideoName, const std::string &path);
};

//template <typename T>
//CaptureVideoFactory * Singleton<T>::_instance = nullptr;
//std::mutex Singleton<CaptureVideoFactory>::_mutex;