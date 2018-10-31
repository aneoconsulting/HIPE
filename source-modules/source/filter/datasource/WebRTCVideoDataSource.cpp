//%HIPE_LICENSE%

#ifdef USE_WEBRTC
#include <datasource/WebRTCVideoDataSource.h>
#include <WebRTCCapturer.h>
#include <glog/logging.h>
#include <corefilter/tools/Localenv.h>

filter::datasource::WebRTCVideoDataSource::WebRTCVideoDataSource(const WebRTCVideoDataSource& left)
{
	this->a_isActive.exchange(left.a_isActive);
	this->atomic_state.exchange(left.atomic_state);
	this->port = left.port;
	this->sourceType = left.sourceType;
	this->loop = left.loop;
	this->eSourceType = left.eSourceType;
	this->video = nullptr;
	this->task = nullptr;
}

void filter::datasource::WebRTCVideoDataSource::captureTasks()
{
	
	task = new std::thread([this]()
	{
		bool firstInit = true;
		a_isActive.exchange(true);
		while (this->a_isActive)
		{
			cv::Mat res;
			int retry = 5;
			if (firstInit)
			{
				retry = 70;
				firstInit = false;
			}

			while (res.empty()) // Timeout to capture image
			{
				res = video->Capture();
				if (!this->a_isActive)
				{
					stack.clear();
					stack.push(res);
					break;
				}

				if (retry <= 0)
				{
					LOG(WARNING) << "Timeout to capture video from WebRTC SOURCE. Stop Capture task";
					stack.clear();
					stack.push(res);
					break;
				}
				retry--;
			}
			if (res.channels() == 4)
			{
				cv::cvtColor(res, res, CV_BGRA2BGR);
			}
			stack.clear();
			stack.push(res);
		}
		LOG(INFO) << "End of Capture task";
	});
}

HipeStatus filter::datasource::WebRTCVideoDataSource::process()
{
	data::ImageData img;
	cv::Mat res;

	if (stack.trypop_until(res, 30000))
	{
		img.getMat() = res;
	}
	if (res.empty())
		return END_OF_STREAM;

	PUSH_DATA(img);

	return OK;
}

void filter::datasource::WebRTCVideoDataSource::dispose()
{
	a_isActive = false;
	if (task != nullptr && task->joinable())
	{
		task->join();
		delete task;
		task = nullptr;
	}

	if (video != nullptr)
	{
		video->stopWebRTCServer();
		delete video;
		video = nullptr;
	}

}

HipeStatus filter::datasource::WebRTCVideoDataSource::intialize()
{
	return OK;
}

void filter::datasource::WebRTCVideoDataSource::onLoad(void* data)
{
	if (!a_isActive.exchange(true))
	{
		std::string workdir = corefilter::getLocalEnv().getValue("workingdir") + "/http-root/certificats";
		std::string base_cert = corefilter::getLocalEnv().getValue("base_cert");
		std::stringstream path;

		path << workdir;

		if (!isDirExist(path.str()))
		{
			throw HipeException("Cannot find the working dir for webrtc");
		}
		std::string built_path = path.str();
		
		
		video = new WebRTCCapturer(port, base_cert.c_str());
		video->startWebRTCServer();
		captureTasks();
	}
}
#endif // USE_WEBRTC
