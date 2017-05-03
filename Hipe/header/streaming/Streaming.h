#pragma once
#include <core/Singleton.h>
#include <map>
#include <core/Logger.h>


class TaskContainer
{
	std::shared_ptr<boost::thread> _thread;
	

public:
	
	int height;
	int width;
	int fps;
	void* server;
	void* encoder;

	void setThread(boost::thread* thread)
	{
		_thread.reset(thread);
	}

	boost::thread* getThread() const
	{
		return _thread.get();
	}

	void startStreaming()
	{
		active = true;
	}

private:
	std::atomic<bool> active;

public:
	TaskContainer() : height(0), width(0), fps(0), active(false)
	{
		
	}

	TaskContainer(const TaskContainer& copy) : height(0), width(0), fps(0), active(false)
	{

		throw HipeException("Not implemented : How can we manage the thread copy and control");
	}

	~TaskContainer()
	{
		stopStreaming();
	}

	std::atomic<bool>& isActive()
	{
		return active;
	}

	bool isRunning() const;

	void stopStreaming();

	bool taskIsJoinable() const;

	void onFrameMethod(cv::Mat& mat);
};


class Streaming : public Singleton<Streaming>
{
	friend class Singleton<Streaming>;

	static core::Logger logger;

	std::map<int, std::shared_ptr<TaskContainer>> streamReverseTasks;
	std::map<std::shared_ptr<TaskContainer>, int> streamsTasks;

public:
	inline bool streamingAlreadyExist(int port)
	{
		//TODO : becarefull this is not yet thread safe
		if (streamReverseTasks.find(port) != streamReverseTasks.end())
		{
			return true;
		}

		return false;
	}

	std::shared_ptr<TaskContainer> & getStreaming(int port, int height, int width, int fps);


	std::shared_ptr<TaskContainer>& createStreaming(int port, int height, int width, int fps);
};
