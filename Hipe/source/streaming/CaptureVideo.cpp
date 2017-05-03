#include <CaptureVideo.h>
#include <RTSPCapture.h>
#include <core/misc.h>

template<> DLL_PUBLIC CaptureVideoFactory * Singleton<CaptureVideoFactory>::_instance = nullptr;
//template<> DLL_PUBLIC std::mutex Singleton<CaptureVideoFactory>::_mutex;

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
