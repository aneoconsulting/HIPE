#pragma once
#include <core/Singleton.h>
#include <map>
#include <thread>
#include <future>
#include <chrono>
#include <core/Logger.h>


class TaskContainer
{
	std::shared_ptr<boost::packaged_task<int>> _task;
	std::shared_ptr<boost::thread> _thread;
	
public:
	boost::packaged_task<int> * getTask() const
	{
		return (_task.get());
	}

	void setTask(boost::packaged_task<int> * packaged_task)
	{
		_task.reset(packaged_task);
	}

	void setThread(boost::thread* thread)
	{
		_thread.reset(thread);
	}

	boost::thread* getThread()
	{
		return _thread.get();
	}
private:
	std::shared_ptr<core::queue::ConcurrentQueue<filter::data::IOData>> _concurrent_queue;

public:
	core::queue::ConcurrentQueue<filter::data::IOData> * getQueue() const
	{
		return _concurrent_queue.get();
	}

	void setQueue(core::queue::ConcurrentQueue<filter::data::IOData> * concurrent_queue)
	{
		_concurrent_queue.reset(concurrent_queue);
	}

private:
	char watchVariable;

public:
	TaskContainer(boost::packaged_task<int> * task,
		core::queue::ConcurrentQueue<filter::data::IOData> * concurrent_queue) : _task(task), _concurrent_queue(concurrent_queue)
	{
		watchVariable = 0;
	}

	TaskContainer(const TaskContainer & copy) : _task(copy._task), _concurrent_queue(copy._concurrent_queue)
	{
		watchVariable = 0;
	}

	~TaskContainer() {}

	char *getWatchVariable() { return & watchVariable; }

	void stopStreaming() { watchVariable = 1; }

};

int startStreaming(int port, TaskContainer * taskContainer);

class Streaming : public Singleton<Streaming>
{

	friend class Singleton<Streaming>;

	static core::Logger logger;

	std::map<int, TaskContainer*> streamReverseTasks;
	std::map<TaskContainer*, int> streamsTasks;

public:
	inline bool streamingAlreadyExist(int port)
	{
		if (streamReverseTasks.find(port) != streamReverseTasks.end())
		{
			return true;
		}

		return false;
	}

	inline bool taskIsReady(boost::packaged_task<int(int, TaskContainer*)>& task)
	{
		
		return false;
	}

	core::queue::ConcurrentQueue<filter::data::IOData> * getStreaming(int port);

	core::queue::ConcurrentQueue<filter::data::IOData> * createStreaming(int port);
};
