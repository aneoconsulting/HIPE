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
#include <core/Singleton.h>
#include <map>
#include <core/Logger.h>
#include <streaming/streaming_export.h>


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

	void onFrameMethod(cv::Mat mat);
};


class STREAMING_EXPORT Streaming : public Singleton<Streaming>
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
