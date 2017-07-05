#pragma once
#include <filter/Model.h>
#include <boost/thread.hpp>
#include <atomic>

namespace orchestrator
{
	class TaskInfo
	{
	public:
		std::shared_ptr<boost::thread> task;
		std::shared_ptr<filter::Model> filter;
		std::shared_ptr<std::atomic<bool> > isActive;

	};
}