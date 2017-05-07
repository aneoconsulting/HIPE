#pragma once
#include <string>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <core/HipeException.h>
#include <map>
#include <mutex>
#include <core/Singleton.h>
#include <streaming/streaming_export.h>

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


class STREAMING_EXPORT CaptureVideoFactory : public Singleton<CaptureVideoFactory>
{

protected:
	std::map<std::string, CaptureVideo *> _captureVideoTable;
	std::mutex locking;
public:
	CaptureVideo* getCaptureVideo(std::string& captureVideoName, const std::string &path);
	
};

