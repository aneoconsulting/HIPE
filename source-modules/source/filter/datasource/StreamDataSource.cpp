//@HIPE_LICENSE@
#include <filter/datasource/StreamDataSource.h>
#include <data/FileVideoInput.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>

std::string buildGstreamUri(int udp_port)
{
	std::stringstream buildUri;

	//udpsrc port=8864 ! application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! timeoverlay shaded-background=true deltay=20 ! appsink sync=false
	buildUri << "udpsrc port=" << udp_port << "  ! application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! timeoverlay shaded-background=true deltay=20 ! appsink sync=false";
	return buildUri.str();
}

HipeStatus filter::datasource::StreamDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		if (!video)
		{

			video = std::make_shared<data::FileVideoInput>(buildGstreamUri(udp_port), false);

		}
	}
	data::Data new_frame = video->newFrame();


	if (!new_frame.empty())
	{
		PUSH_DATA(new_frame);

		return OK;
	}

	video.reset();

	return END_OF_STREAM;

	PUSH_DATA(data::ImageData());
	return END_OF_STREAM;
}

HipeStatus filter::datasource::StreamDataSource::intialize()
{
	if (!atomic_state.exchange(true))
	{
		if (!video)
		{
			video = std::make_shared<data::FileVideoInput>(buildGstreamUri(udp_port), false);
		}
	}
	data::Data new_frame = video->newFrame();
	video->closeFile();

	return OK;
}



