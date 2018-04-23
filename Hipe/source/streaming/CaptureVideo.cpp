//@HIPE_LICENSE@
#include <CaptureVideo.h>
#include <RTSPCapture.h>
#include <core/Singleton.h>
#include <streaming/streaming_export.h>
#include <stdexcept>

template<> CaptureVideoFactory * Singleton<CaptureVideoFactory>::_instance = nullptr;


 CaptureVideo* CaptureVideoFactory::getCaptureVideo(std::string& captureVideoName, const std::string &path)
{
	CaptureVideoFactory *captureVideoFactory = CaptureVideoFactory::getInstance();

	
	
	if (captureVideoName == "RTSPCapture")
	{
		captureVideoFactory->locking.lock();
		if (captureVideoFactory->_captureVideoTable.find(captureVideoName) == captureVideoFactory->_captureVideoTable.end())
			captureVideoFactory->_captureVideoTable[captureVideoName] = new RTSPCapture(path);
		
		captureVideoFactory->locking.unlock();
	}
	else
	{
		captureVideoFactory->locking.unlock();
		throw HipeException("No captureVideo IOData is named : " + captureVideoName);
	}

	CaptureVideo* capture_video = captureVideoFactory->_captureVideoTable[captureVideoName];

	return capture_video;
}
