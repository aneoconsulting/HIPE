//%HIPE_LICENSE%
#ifdef USE_WEBRTC
#include <core/misc.h>

#include <filter/algos/streaming/WebRTCSender.h>
#include <opencv2/opencv.hpp>

HipeStatus WebRTCSender::process()
{
	while (_connexData.size() != 0)
	{
		data::ImageData img;
		img = _connexData.pop();
		if (streamer != nullptr)
		{
			cv::Mat res;
			switch (img.getMat().channels())
			{
			case 1:
				cv::cvtColor(img.getMat(), res, CV_GRAY2BGRA);
				break;
			case 3:
				cv::cvtColor(img.getMat(), res, CV_BGR2BGRA);
			break;

			case 4:
				res = img.getMat();
			break;
			default:
				throw HipeException("UNconvertible number of  channels for WebRTC");

			};
			sendNewFrame(streamer, res.ptr(), res.cols, res.rows, res.channels());
		}
			
	}

	return OK;
}

void WebRTCSender::onLoad(void* data)
{
	if (!isStart.exchange(true))
	{
		const char *workdir = "http-root/certificats";
		std::stringstream path;
		
		path << GetCurrentWorkingDir() << PathSeparator() << workdir;

		if (! isDirExist(path.str()))
		{
			throw HipeException("Cannot find the working dir for webrtc");
		}
		std::string built_path = path.str();
		const char* c_str = built_path.c_str();
		streamer = newWebRTCStreamer(port, c_str);
		startStreamerServer(streamer);
	}
}

void WebRTCSender::dispose()
{
	if (isStart.exchange(false))
	{
		deleteWebRTCStreamer(&streamer);
		streamer = nullptr;
	}
}

#endif
