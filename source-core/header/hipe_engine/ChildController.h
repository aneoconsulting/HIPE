#pragma once
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/pair.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/process.hpp>
#include "core/HipeException.h"
#include "corefilter/tools/Localenv.h"
#include "json/JsonTree.h"
#include "core/Logger.h"

class ChildProcess
{
public:
	typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>
	CharAllocator;
	typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> MyShmString;
	typedef boost::interprocess::allocator<MyShmString, boost::interprocess::managed_shared_memory::segment_manager>
	StringAllocator;

	typedef std::pair<const MyShmString, MyShmString> ValueType;
	typedef MyShmString MappedType;
	typedef MyShmString KeyType;

	typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager>
	ShmemAllocator;

	typedef boost::interprocess::map<KeyType, MappedType, std::less<MyShmString>, ShmemAllocator> MyMap;

	//Initialize the shared memory STL-compatible allocator

public:
	ChildProcess() : isActive(false)
	{
		try {
		segment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_only, "MySharedMemory");
		}
		catch (boost::interprocess::interprocess_exception & ex)
		{
			LOG(ERROR) << std::string("Fail to get SharedMemory : ") + ex.what() << std::endl;
			throw HipeException(std::string("Fail to get SharedMemory : ") + ex.what());
		}

		//segment size in bytes

		alloc_inst = new ShmemAllocator(segment->get_segment_manager());

		charallocator = new CharAllocator(segment->get_segment_manager());

		mtx = segment->find_or_construct<boost::interprocess::interprocess_mutex>("mtx")();

		//Get request first
		//auto shRequest = segment->find<MyShmString>("ShRequest");

		auto pair = segment->find<MyMap>("LocalEnv");
		mymap = pair.first;

		shRequest = segment->find_or_construct<MyShmString>("ShRequest")(
					"", segment->get_segment_manager());
		shResponse = segment->find_or_construct<MyShmString>("ShResponse")(
					"", segment->get_segment_manager());
	}

	~ChildProcess()
	{
		if (segment != nullptr)
		{
			mtx->unlock(); 
			delete charallocator;
			delete alloc_inst;
			delete mtx;
			delete segment;
			segment = nullptr;

			LOG(INFO) << "Shared Segment is removed from child" << std::endl;
		}
	}

	void copyLocalEnv()
	{
	}

	void startProcess(std::function<json::JsonTree(json::JsonTree treeRequest)> tasker);

private:
	boost::interprocess::managed_shared_memory* segment;
	//Initialize the shared memory STL-compatible allocator
	ShmemAllocator* alloc_inst;

	CharAllocator* charallocator;
	boost::interprocess::interprocess_mutex* mtx;

	MyMap* mymap;

	std::atomic<bool> isActive;

	MyShmString * shRequest;
	MyShmString * shResponse ;
};
