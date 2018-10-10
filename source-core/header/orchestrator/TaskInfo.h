//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#pragma warning(push, 0)
#include <boost/thread.hpp>
#pragma warning(pop)
#include <atomic>
#include <exception>
#include <stdexcept>

namespace orchestrator
{
	class TaskInfo
	{
	public:
		std::shared_ptr<boost::thread> task;
		std::shared_ptr<filter::Model> filter;
		std::shared_ptr<std::atomic<bool> > isActive;
		std::shared_ptr<std::exception_ptr> texptr;
		
		TaskInfo()
		{
			
		}

	};
}