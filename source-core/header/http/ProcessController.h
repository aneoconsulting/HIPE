#pragma once


#ifndef BOOST_INTERPROCESS_SHARED_DIR_FUNC
#define BOOST_INTERPROCESS_SHARED_DIR_FUNC

#include <string>
#include <core/misc.h>
#include <glog/logging.h>

namespace boost {
    namespace interprocess {
        namespace ipcdetail {
            inline void get_shared_dir(std::string &shared_dir)
			{
				shared_dir = getEnv("TEMP");
			}
        }
    }
}
#endif

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/process.hpp>
#include "core/HipeException.h"
#include "corefilter/tools/Localenv.h"

class ProcessController
{
public:
	typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>
	CharAllocator;
	typedef std::basic_string<char, std::char_traits<char>, CharAllocator> MyShmString;
	typedef boost::interprocess::allocator<MyShmString, boost::interprocess::managed_shared_memory::segment_manager>
	StringAllocator;

	typedef std::pair<const MyShmString, MyShmString> ValueType;
	typedef MyShmString MappedType;
	typedef MyShmString KeyType;

	typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager>
	ShmemAllocator;

	typedef std::map<KeyType, MappedType, std::less<MyShmString>, ShmemAllocator> MyMap;

	//Initialize the shared memory STL-compatible allocator

	struct shm_remove
	{
		shm_remove();

		~shm_remove();
	};

public:
	ProcessController();

	~ProcessController();

	void tranfertLocalEnv();

	bool checkIfProcessExistOrStillAlive();

	std::string executeOrUpdateAttachedProcess(std::string request);

	void kill_process();
	std::string executeOrUpdateProcess(std::string request);


private:
	boost::interprocess::managed_shared_memory* segment;
	//Initialize the shared memory STL-compatible allocator
	ShmemAllocator* alloc_inst;

	CharAllocator* charallocator;
	boost::interprocess::interprocess_mutex *mtx;

	MyShmString * shRequest;
	MyShmString * shResponse;
	MyMap *mymap;

	shm_remove remover;
	boost::process::child * proc;
	std::mutex locker;

};

ProcessController *getProcessController();