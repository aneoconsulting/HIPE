#include <http/ProcessController.h>
#include "json/JsonTree.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/interprocess/containers/string.hpp>

ProcessController::shm_remove::shm_remove()
{
	boost::interprocess::shared_memory_object::remove("MySharedMemory");
}

ProcessController::shm_remove::~shm_remove()
{
	boost::interprocess::shared_memory_object::remove("MySharedMemory");
	LOG(INFO) << "Remove SharedMemory";
}

ProcessController::ProcessController()
{
	segment = new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, "MySharedMemory",
	                                                         655360); //segment size in bytes
	alloc_inst = new ShmemAllocator(segment->get_segment_manager());

	charallocator = new CharAllocator(segment->get_segment_manager());

	mtx = segment->find_or_construct<boost::interprocess::interprocess_mutex>("mtx")();

	shRequest = segment->construct<MyShmString>("ShRequest")(*charallocator);

	shResponse = segment->construct<MyShmString>("ShResponse")(*charallocator);

	//Construct a shared memory map.
	//Note that the first parameter is the comparison function,
	//and the second one the allocator.
	//This the same signature as std::map's constructor taking an allocator
	mymap =
		segment->construct<MyMap>("LocalEnv") //object name
		(std::less<MyShmString>() //first  ctor parameter
		 , *alloc_inst); //second ctor parameter
	shRequest->assign("");
	tranfertLocalEnv();
	std::vector<std::string> args;
	args.push_back("-s");
	args.push_back("MySharedMemory");
	args.push_back("-m");
	args.push_back(corefilter::getLocalEnv().getValue("modulePath"));

	std::string bin_child = "hipe_engine";
#ifndef WIN32
	bin_child += ".bin";
#endif
	proc = new boost::process::child(boost::process::search_path(bin_child), boost::process::args(args));
}

ProcessController::~ProcessController()
{
	if (proc != nullptr)
	{
		if (proc->running())
		{
			proc->terminate();
		}
		delete proc;
		proc = nullptr;
	}
	delete charallocator;
	delete alloc_inst;
	delete segment;
}

void ProcessController::tranfertLocalEnv()
{
	//Prepare to transfert localEnv data
	for (auto pair : corefilter::getLocalEnv().env_values())
	{
		//Insert data in the map
		MyShmString env_value(*charallocator);
		MyShmString env_key(*charallocator);
		env_key = pair.first.c_str();
		env_value = pair.second.c_str();
		if (mymap->find(env_key) == mymap->end())
			mymap->insert(std::pair<KeyType, MappedType>(env_key, env_value));
	}
}

bool ProcessController::checkIfProcessExistOrStillAlive()
{
	if (proc == nullptr) return false;

	if (!proc->running())
	{
		delete proc;
		proc = nullptr;
		return false;
	}

	return true;
}

std::string ProcessController::executeOrUpdateAttachedProcess(std::string request)
{
	int retry = 10000;
	mtx->lock();
	//Prepare to send request data

	shRequest->assign(request.c_str());
	shResponse->assign("");

	tranfertLocalEnv();

	mtx->unlock();
	//need to check if the request has been disgested by the child. Rule is to waif for string is empty after it try to lock;
	while (retry > 0 && !shRequest->empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		retry--;
	}
	if (!shRequest->empty() || retry <= 0)
	{
		throw HipeException("Something goes wrong in the child process");
	}

	if (shRequest->empty())
	{
		mtx->lock(); //Wait for client response
	}

	std::string response = shResponse->c_str();


	json::JsonTree treeResponse;
	std::stringstream buffer;
	buffer.str(response);
	treeResponse.read_json(buffer);
	
	boost::property_tree::ptree* ptree = treeResponse.getPtree().get();

	ptree->erase("errCode");
	buffer.str("");
	treeResponse.write_json(buffer);
	response = buffer.str();

	shRequest->assign("");
	shResponse->assign("");

	mtx->unlock(); //Wait for client response

	return response;
}

void ProcessController::kill_process()
{
	if (proc == nullptr) return;

	if (proc->running())
	{
		proc->terminate();

		delete proc;
		proc = nullptr;
	}
}

void ProcessController::start_process()
{
	tranfertLocalEnv();
		std::vector<std::string> args;
		args.push_back("-s");
		args.push_back("MySharedMemory");
		args.push_back("-m");
		args.push_back(corefilter::getLocalEnv().getValue("modulePath"));

		std::string bin_child = "hipe_engine";
#ifndef WIN32
		bin_child += ".bin";
#endif
		
		proc = new boost::process::child(boost::process::search_path(bin_child), boost::process::args(args));
}

std::string ProcessController::executeOrUpdateProcess(std::string request)
{
	std::lock_guard<std::mutex> lock(locker);
	int retry = 600;
	mtx->lock();
	/*LOG(INFO) << "Parent : mutex lock" << std::endl;*/
	//Prepare to send request data
	shRequest->assign(request.c_str());
	shResponse->assign("");


	if (! checkIfProcessExistOrStillAlive())
	{
		start_process();
		
	}

	mtx->unlock();
	/*LOG(INFO) << "Parent : mutex unlock" << std::endl;*/

	//need to check if the request has been disgested by the child. Rule is to waif for string is empty after it try to lock;
	while (retry > 0 && proc->running() && !shRequest->empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		retry--;
	}
	if ((!proc->running() && !shRequest->empty()) || retry <= 0)
	{
		kill_process();
		throw HipeException("Something goes wrong in the child process");
	}
	

	std::string response = "";
	try
	{
		if (proc->running() && shRequest->empty())
		{
			int retry_mtx = 5;
			while (!mtx->try_lock() && retry_mtx > 0)
			{
				retry_mtx--;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}

			if (retry <= 0 && mtx->try_lock() == false)
			{
				kill_process();
				start_process();
			}
			/*LOG(INFO) << "Parent : mutex lock" << std::endl;*/
			response = shResponse->c_str();

			json::JsonTree treeResponse;
			std::stringstream buffer;
			buffer.str(response);
			treeResponse.read_json(buffer);
			std::string errCode = treeResponse.get<std::string>("errCode");
			boost::property_tree::ptree* ptree = treeResponse.getPtree().get();
			shRequest->assign("");

			mtx->unlock(); //Wait for client response
			/*LOG(INFO) << "Parent : mutex unlock" << std::endl;*/

			if (errCode != "200") {
				kill_process();
				start_process();
			}

			ptree->erase("errCode");
			buffer.str("");
			treeResponse.write_json(buffer);
			response = buffer.str();
		}
		else
		{
			LOG(INFO) << "Fail to lock sharedMemory" << std::endl;
		}

		if (! checkIfProcessExistOrStillAlive())
		{
			std::thread restartIfNeeded = std::thread([&]()
			{
					start_process();
			});
			restartIfNeeded.detach();
		}
	}
	catch (std::exception& e)
	{
		mtx->unlock();
		/*LOG(INFO) << "Parent : mutex unlock" << std::endl;*/
		LOG(INFO) << "Parent : fail to receive message : " << e.what() << std::endl;
		kill_process();

	}

	return response;
}

ProcessController* getProcessController()
{
	static ProcessController* process_controller = new ProcessController();

	return process_controller;
}
