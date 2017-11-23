#include <UsageEnvironment.hh>
#include <BasicUsageEnvironment.hh>
#include <RTSPServer.hh>
#include <string>
#include <H264LiveServerMediaSession.h>

#include <future>
#include <thread>
#include <chrono>
#include <Streaming.h>
#include <streaming_rtsp/LiveRTSPServer.h>
#include "streaming_export.h"

core::Logger Streaming::logger = core::setClassNameAttribute("Streaming");

int startStreaming(int port, TaskContainer* taskContainer)
{
	/*TaskScheduler* taskSchedular = BasicTaskScheduler::createNew();
	BasicUsageEnvironment* usageEnvironment = BasicUsageEnvironment::createNew(*taskSchedular);
	RTSPServer* rtspServer = RTSPServer::createNew(*usageEnvironment, port, nullptr);
	if (rtspServer == nullptr)
	{
		*usageEnvironment << "Failed to create rtsp server ::" << usageEnvironment->getResultMsg() << "\n";
		exit(1);
	}

	std::string streamName = "usb1";
	ServerMediaSession* sms = ServerMediaSession::createNew(*usageEnvironment, streamName.c_str(), streamName.c_str(), "Live H264 Stream");
	H264LiveServerMediaSession* liveSubSession = H264LiveServerMediaSession::createNew(*usageEnvironment, true, *taskContainer->getQueue());
	sms->addSubsession(liveSubSession);
	rtspServer->addServerMediaSession(sms);
	char* url = rtspServer->rtspURL(sms);
	*usageEnvironment << "Play the stream using url " << url << "\n";
	delete[] url;
	taskSchedular->doEventLoop(taskContainer->getWatchVariable());*/

	return 0;
}

#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))


void aligned2RBGLine(std::shared_ptr<TaskContainer> taskContainer)
{
	taskContainer->width = FFALIGN(taskContainer->width, 2);
	//taskContainer->width = FFALIGN(taskContainer->width, 24);
	taskContainer->height = FFALIGN(taskContainer->height, 2);
}

int startStreamingFFmpeg(int port, std::shared_ptr<TaskContainer> taskContainer)
{
	int UDPPort = port;

	int HTTPTunnelPort = 0;
	MESAI::FFmpegH264Encoder* encoder = new MESAI::FFmpegH264Encoder();

	aligned2RBGLine(taskContainer);

	encoder->SetupVideo("dummy.avi", taskContainer->width, taskContainer->height, taskContainer->fps, 1, taskContainer->width * taskContainer->height * taskContainer->fps);

	taskContainer->encoder = (void *)encoder;


	MESAI::LiveRTSPServer* server = new MESAI::LiveRTSPServer(encoder, UDPPort, HTTPTunnelPort);
	taskContainer->server = (void *)server;

	boost::thread thr_server = boost::thread([server]
		{
			((MESAI::LiveRTSPServer *) server)->run();
		});
	boost::thread thr_encoder = boost::thread([encoder]
		{
			((MESAI::FFmpegH264Encoder *) encoder)->run();
		});

	taskContainer->startStreaming();

	while (taskContainer->isActive())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(300));
	}

	server->quit = 1;
	thr_server.join();

	encoder->quit = true;
	thr_encoder.join();

	return 0;
}

bool TaskContainer::isRunning() const
{
	bool try_join_for = _thread->try_join_for(boost::chrono::microseconds(3));


	if (try_join_for) // the task has finished. Be sure to be clean before stating another one
	{
		return false;
	}

	return taskIsJoinable();
}

void TaskContainer::stopStreaming()
{
	active = false;

	if (isRunning())
	{
		_thread->interrupt();
		_thread->join();
	}
	else
	{
		if (!taskIsJoinable())
		{
			_thread->interrupt();
		}
	}
}

bool TaskContainer::taskIsJoinable() const
{
	return _thread->joinable();
}

void TaskContainer::onFrameMethod(cv::Mat mat)
{
	static_cast<MESAI::FFmpegH264Encoder*>(encoder)->SendNewFrame(mat);
}

std::shared_ptr<TaskContainer>& Streaming::getStreaming(int port, int height, int width, int fps)
{
	//TODO check if the function need to be locked from re-entrance

	if (streamingAlreadyExist(port))
	{
		std::shared_ptr<TaskContainer>& container = streamReverseTasks[port];

		if (!container->isRunning()) // the task isn't joinable !!! kill it
		{
			container->stopStreaming();

			streamsTasks.erase(container);
			streamReverseTasks.erase(port);

			//TODO check if the old container delete the task

			return createStreaming(port, height, width, fps);
		}
		return container;
	}


	return createStreaming(port, height, width, fps);
}


std::shared_ptr<TaskContainer>& Streaming::createStreaming(int port, int height, int width, int fps)
{
	//core::queue::ConcurrentQueue<data::IOData>* concurrent_queue = new core::queue::ConcurrentQueue<data::IOData>();
	std::shared_ptr<TaskContainer> shrTaskContainer = std::make_shared<TaskContainer>();
	int rows = (height >> 1) * 2;
	int cols = (width / 24) * 24;

	if (height != rows) height = rows + 2;
	if (width != cols) width = cols + 24;

	//FFmpeg need a multiple of 2 as Image Dimension so we fix now the dimension and in SendNewFrame we create a new frame with this dimension

	shrTaskContainer->height = height;
	shrTaskContainer->width = width;
	shrTaskContainer->fps = fps;

	// Run task .
	// Run task on new thread.
	boost::thread* t = new boost::thread(boost::bind(startStreamingFFmpeg, port, shrTaskContainer));
	shrTaskContainer->setThread(t);

	streamReverseTasks[port] = shrTaskContainer;
	streamsTasks[shrTaskContainer] = port;

	return streamReverseTasks[port];
}

template <> Streaming* Singleton<Streaming>::_instance = nullptr;

