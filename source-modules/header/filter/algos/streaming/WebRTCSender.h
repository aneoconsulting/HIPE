//%HIPE_LICENSE%
#pragma once

#ifdef USE_WEBRTC
#include <corefilter/tools/RegisterClass.h>

#include <corefilter/filter_export.h>
#include <corefilter/IFilter.h>
#include <coredata/NoneData.h>
#include <data/ImageData.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>
#include <WebRTCStreamer.h>

class FILTER_EXPORT WebRTCSender : public filter::IFilter
{

public:
	SET_NAMESPACE("vision/streaming")

	CONNECTOR(data::ImageData, data::NoneData);

	REGISTER(WebRTCSender, ()), _connexData(data::INDATA)
	{
		isStart = false;
		streamer = nullptr;
	}

	std::atomic<bool> isStart;
	void* streamer;
	REGISTER_P(int, port);

	HipeStatus process();


	virtual void onLoad(void* data);

	virtual void dispose();;

};

ADD_CLASS(WebRTCSender, port);
#endif // USE_WEBRTC