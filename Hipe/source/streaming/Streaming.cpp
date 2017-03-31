#include <UsageEnvironment.hh>
#include <BasicUsageEnvironment.hh>
#include <RTSPServer.hh>
#include <string>
#include <H264LiveServerMediaSession.h>

#include <future>
#include <thread>
#include <chrono>
#include <Streaming.h>

core::Logger Streaming::logger = core::setClassNameAttribute("Streaming");

int startStreaming(int port, TaskContainer* taskContainer)
{
	TaskScheduler* taskSchedular = BasicTaskScheduler::createNew();
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
	taskSchedular->doEventLoop(taskContainer->getWatchVariable());

	return 0;
}

core::queue::ConcurrentQueue<filter::data::IOData>* Streaming::getStreaming(int port)
{
	//TODO check if the function need to be locked from re-entrance

	if (streamingAlreadyExist(port))
	{
		auto container = streamReverseTasks[port];
		auto concurrent_queue = container->getQueue();

		//boost::packaged_task<int>* task = container->getTask();

		boost::thread* thr = container->getThread();
		bool try_join_for = thr->try_join_for(boost::chrono::microseconds(0));

		if (try_join_for)
		{
			container->stopStreaming();
			

			streamsTasks.erase(container);
			streamReverseTasks.erase(port);
			
			delete container;
			return createStreaming(port);
		}
		return concurrent_queue;
	}

	return createStreaming(port);
}

core::queue::ConcurrentQueue<filter::data::IOData>* Streaming::createStreaming(int port)
{
	core::queue::ConcurrentQueue<filter::data::IOData>* concurrent_queue = new core::queue::ConcurrentQueue<filter::data::IOData>();

	// Create a packaged_task using some task and get its future.
	//boost::packaged_task<int(int, TaskContainer *)>* task = new boost::packaged_task<int(int, TaskContainer *)>(std::bind(startStreaming, port, nullptr));

	TaskContainer* taskContainer = new TaskContainer(nullptr, concurrent_queue);

	// Run task .
	// Run task on new thread.
	boost::thread* t = new boost::thread(boost::bind(startStreaming, port, taskContainer));
	taskContainer->setThread(t);

	streamReverseTasks[port] = taskContainer;
	streamsTasks[taskContainer] = port;

	return taskContainer->getQueue();
	return nullptr;
}

template <>
Streaming* Singleton<Streaming>::_instance = nullptr;

std::mutex BaseSingleton::_mutex;
