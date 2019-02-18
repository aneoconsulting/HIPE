//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
