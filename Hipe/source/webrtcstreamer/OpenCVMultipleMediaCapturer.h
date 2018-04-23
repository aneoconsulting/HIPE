//@HIPE_LICENSE@
#pragma once
#include <scy/webrtc/multiplexmediacapturer.h>
#include "OCVVideoCapture.h"


class OpenCVMultipleMediaCapturer : public scy::MultiplexMediaCapturer
{
public:
	OpenCVMultipleMediaCapturer()
	{
		_stream.reset();
		_videoCapture = (std::make_shared<OCVVideoCapture>());
		_audioModule = (scy::AudioPacketModule::Create());
		_stream.attachSource(_videoCapture, true);
		
		_stream.emitter += packetSlot(_audioModule.get(), &scy::AudioPacketModule::onAudioCaptured);
		
	}

	virtual ~OpenCVMultipleMediaCapturer()
	{
		
	}

	virtual void openFile(const std::string& file, bool loop = true)
	{
		//TODO manage looping no need ATM
	}

	virtual core::queue::ConcurrentQueue<cv::Mat> & getQueueMat()
	{
		std::shared_ptr<OCVVideoCapture> derived =
			std::dynamic_pointer_cast<OCVVideoCapture> (_videoCapture);
		return derived.get()->getQueueMat();
	}

	virtual void openStream(const std::string& filename, AVInputFormat* inputFormat, AVDictionary** formatParams)
	{
		
	}
	
};
