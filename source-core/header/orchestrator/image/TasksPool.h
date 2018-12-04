//@HIPE_LICENSE@
#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

namespace orchestrator
{
	namespace image
	{
		typedef boost::packaged_task<int> task_t;
		typedef boost::shared_ptr<task_t> ptask_t;

		class TasksPool
		{
			boost::asio::io_service io_service;
		public:
			boost::asio::io_service & getIoService() 
			{
				return io_service;
			}

		private:
			boost::thread_group threads;
			//boost::asio::io_service::work work(io_service);
			std::vector<boost::shared_future<int> > pending_data; // vector of futures

			
		public:
			TasksPool()
			{
				for (int i = 0; i < boost::thread::hardware_concurrency(); ++i)
				{
					threads.create_thread(boost::bind(&boost::asio::io_service::run,
						&io_service));

				}
			}
			~TasksPool()
			{
				threads.interrupt_all();
			}

			void push_job(ptask_t task, std::vector<boost::shared_future<int> >& pending_data) {
				//ptask_t task = boost::make_shared<task_t>(boost::bind(&sleep_print, seconds));
				boost::shared_future<int> fut(task->get_future());
				pending_data.push_back(fut);
				io_service.post(boost::bind(&task_t::operator(), task));
			}

		};
	}
}