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
#include <queue>
#include <atomic>
#pragma warning(push, 0)
#include <boost/thread.hpp>
#pragma warning(pop)

namespace core
{
	namespace queue
	{
		template<typename Data>
		class ConcurrentQueue
		{
		private:
			std::queue<Data> the_queue;
			mutable boost::mutex the_mutex;
			boost::condition_variable the_condition_variable;
			std::atomic<bool> _listerners;
		public:
			ConcurrentQueue() { _listerners = false; }

			void stopListening()
			{
				_listerners = false;
			}

			~ConcurrentQueue()
			{
				_listerners = false;
			};

			void push(Data const& data)
			{
				boost::mutex::scoped_lock lock(the_mutex);
				the_queue.push(data);
				lock.unlock();
				the_condition_variable.notify_one();
			}

			bool empty() const
			{
				boost::mutex::scoped_lock lock(the_mutex);
				return the_queue.empty();
			}

			size_t size() 
			{
				boost::mutex::scoped_lock lock(the_mutex);
				return the_queue.size();
			}

			void clear()
			{
				boost::mutex::scoped_lock lock(the_mutex);
				std::queue<Data> empty;
				std::swap(the_queue, empty);
			}

			bool pop(Data& popped_value)
			{
				return try_pop(popped_value);
			}

			bool try_pop(Data& popped_value)
			{
				boost::mutex::scoped_lock lock(the_mutex);
				if (the_queue.empty())
				{
					return false;
				}

				popped_value = the_queue.front();
				the_queue.pop();
				return true;
			}

			void wait_and_pop(Data& popped_value)
			{
				boost::mutex::scoped_lock lock(the_mutex);
				while (the_queue.empty())
				{
					the_condition_variable.wait(lock);
					
				}

				popped_value = the_queue.front();
				the_queue.pop();
			}

			bool waituntil_and_pop(Data& popped_value)
			{
				boost::mutex::scoped_lock lock(the_mutex);
				while (the_queue.empty())
				{
					the_condition_variable.timed_wait(lock, boost::posix_time::milliseconds(1000));
					if (!hasListener()) 
						return false;
				}

				popped_value = the_queue.front();
				the_queue.pop();
				return true;
			}

			bool trypop_until(Data& popped_value, int ms)
			{
				boost::mutex::scoped_lock lock(the_mutex);
				the_condition_variable.timed_wait(lock, boost::posix_time::milliseconds(ms));
				if (the_queue.empty())
				{
					return false;
				}

				popped_value = the_queue.front();
				the_queue.pop();
				return true;
			}
			
			void readyToListen() { _listerners = true; }

			bool hasListener() { return _listerners; }

		};
	}
}
