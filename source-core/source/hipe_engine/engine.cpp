//@HIPE_LICENSE@
//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
//Added for the default_resource example
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <vector>
#include <algorithm>
#include <orchestrator/Orchestrator.h>
#include <core/Logger.h>
#include <core/version.h>
#include <core/ModuleLoader.h>
#include <glog/logging.h>


#include <hipe_engine/Configuration.h>
#include <corefilter/tools/Localenv.h>
#include "corefilter/tools/net/ForwardLogToWeb.h"
#include "corefilter/tools/JsonBuilder.h"
#include "orchestrator/Composer.h"
#include "corefilter/tools/CommandManager.h"
#include <json/JsonTree.h>


#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>
#include <functional>
#include <utility>
#include <fstream>

#pragma warning(pop)

#ifndef BOOST_INTERPROCESS_SHARED_DIR_FUNC
#define BOOST_INTERPROCESS_SHARED_DIR_FUNC
#endif
namespace boost
{
	namespace interprocess
	{
		namespace ipcdetail
		{
			void get_shared_dir(std::string& shared_dir)
			{
				shared_dir = getEnv("TEMP");
			}
		}
	}
}

#include "ChildController.h"

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

using namespace std;
//Added for the json-example:

void fail_func()
{
	LOG(INFO) << "OK I'm back";
}

std::vector<std::string> defaultListWorkingDirectory()
{
	std::vector<std::string> relativePath;
	string cs = getEnv("HIPE_HOME");

	if (!cs.empty())
	{
		relativePath.push_back(cs);
	}
	//If The binary was runned from workingDir
	relativePath.push_back("../workingdir");

	//If the binary was runned from core binary folder
	relativePath.push_back("../../workingdir");

	//If The binary was runned from one of modules
	relativePath.push_back("../../../workingdir");


	return relativePath;
}

void add_python_path(const std::string& python_path)
{
	std::vector<std::string> results;

	boost::split(results, python_path, [](char c) { return c == ';'; });


	for (auto p : results)
	{
		addEnv(p);
		addVarEnv("PYTHONPATH", p);
		LOG(INFO) << "Add Python path : " << p.c_str() << std::endl;
	}
}

void setPythonPath()
{
	add_python_path(corefilter::getLocalEnv().getValue("python_dll_path"));
}

net::log::ForwardLogToWeb* startLogService()
{
	net::log::ForwardLogToWeb* logForward = new net::log::ForwardLogToWeb();
	logForward->setName(std::string("Base_log_forward"));
	logForward->setLevel(0);
	int port = 9135; // it's the children
	logForward->set_port(port);
	try
	{
		logForward->onLoad(nullptr);
	}
	catch (const websocketpp::exception &e)
	{//by safety, I just go with `const std::exception` so that it grabs any potential exceptions out there.
		LOG(WARNING) << "hipe_engine : Exception in method onload() because: " << e.what() /* log the cause of the exception */ << std::endl;
	}


	return logForward;
}

json::JsonTree executeJson(json::JsonTree& treeRequest)
{
	json::JsonTree treeResponse;
	try
	{
		stringstream dataResponse;
		json::JsonTree treeResponseInfo;

		if (treeRequest.count("command") != 0)
		{
			std::string result;
			std::stringstream debug;
			auto command = treeRequest.get_child("command").get<std::string>("type");
			json::JsonTree ltreeResponse; // = new JsonTree;

			debug.str("");
			ltreeResponse.write_json(debug);
			result = debug.str();
			auto killCommandFound = corefilter::CommandManager::callOption(
				command, orchestrator::kill_command(), &ltreeResponse);
			killCommandFound |= corefilter::CommandManager::callOption(command, orchestrator::exit_command(),
			                                                           &ltreeResponse);

			auto commandFound = corefilter::CommandManager::callOption(command, corefilter::get_version(),
			                                                           &ltreeResponse);


			commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_filters(), & ltreeResponse);

			commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_versionHashed(),
			                                                       &ltreeResponse);

			commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_commands_help(),
			                                                       & ltreeResponse);

			commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_groupFilter(),
			                                                       &ltreeResponse);


			if (!commandFound && !killCommandFound)
			{
				LOG(INFO) << "command " << command << " not found", command;

				ltreeResponse.Add(command, "error:  command not found");
			}


			LOG(INFO) << "HttpTask response has been sent";

			if (killCommandFound)
			{
				LOG(INFO) << "Exit is required. Restart a new process now!";
				orchestrator::OrchestratorFactory::getInstance()->killall();
				throw HipeException("Exit is required. Restart a new process now");
			}
			ltreeResponse.Add("errCode", "200");
			return ltreeResponse;
		}

		LOG(INFO) << "Check if algorithm need to be built";


		auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
		const string name = json_filter_tree->getName();
		orchestrator::OrchestratorFactory::getInstance()->addModel(name, json_filter_tree);

		json_filter_tree = static_cast<json::JsonFilterTree *>(orchestrator::OrchestratorFactory::getInstance()->
			getModel(name));
		if (json_filter_tree == nullptr)
			throw HipeException("fail to build or get the model");

		treeResponseInfo.Add("Algorithm", dataResponse.str());
		dataResponse.str(std::string());

		LOG(INFO) << "Check if orchestrator need to be built";
		auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
		treeResponseInfo.Add("Orchestrator", dataResponse.str());
		dataResponse.str(std::string());

		stringstream strlog;
		strlog << "Bind algorithm ";
		strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

		VLOG(3) << strlog.str();

		orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
		treeResponseInfo.Add("Binding", "OK");
		treeResponse.add_child("Status", treeResponseInfo);

		std::stringstream status;
		treeResponseInfo.write_json(status);
		VLOG(3) << "Response info :\n" << status.str();

		//Check if data is present
		if (treeRequest.count("data") != 0)
		{
			boost::property_tree::ptree& pt = treeRequest.get_child("data");
			json::JsonTree jtr(pt);
			auto data = orchestrator::Composer::getDataFromComposer(jtr);
			//Start processing Algorithm with data
			data::Data outputData;

			orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

			//after the process execution Data should be an OutputData type
			if (outputData.getType() == data::IMGB64)
			{
				throw HipeException(
					"Need to rethink the output of a image. Since there is a core data. The child of OutputData should use JsonTree directly in the filter modules");
			}
		}
		treeResponse.Add("errCode", "200");
		treeResponse.write_json(dataResponse);

		return treeResponse;
	}
	catch (HipeException& e)
	{
		int errorCode = 400;
		std::string errorMsg = std::string(e.what());

	
		std::stringstream conv;
		conv << errorCode;
		treeResponse.Add("errCode", conv.str());
		treeResponse.Add("Status", e.what());
		std::stringstream dataResponse;
		treeResponse.write_json(dataResponse);

		LOG(ERROR) << "HipeException : " << e.what();
		PRINTSTACK(INFO);
		return treeResponse;
	}
	catch (std::exception& e)
	{
		treeResponse.Add("Status", e.what());
		treeResponse.Add("errCode", "400");
		std::stringstream dataResponse;
		treeResponse.write_json(dataResponse);

		LOG(ERROR) << "std::exception : " << e.what();
		PRINTSTACK(INFO);
		return treeResponse;
	}
	catch (...)
	{
		treeResponse.Add("Status", "Uknown exception");
		treeResponse.Add("errCode", "400");
		std::stringstream dataResponse;
		treeResponse.write_json(dataResponse);


		LOG(ERROR) << "Unkown exception in hipe_engine...";
		PRINTSTACK(INFO);
		return treeResponse;
	}

	return treeResponse;
}

void executeFromJsonFile(const string& json_request_file, int wait_ms)
{
	//Write file with code
	std::ifstream myfile;
	isFileExist(json_request_file);

	myfile.open(json_request_file);
	std::stringstream buffer;
	buffer << myfile.rdbuf();

	myfile.close();

	json::JsonTree treeRequest;
	treeRequest.read_json(buffer);
	//std::atomic<bool> isActive;
	//core::queue::ConcurrentQueue<json::JsonTree> request_queue;
	//core::queue::ConcurrentQueue<json::JsonTree> response_queue;

	//isActive = true;

	json::JsonTree treeResponse = executeJson(treeRequest);

	std::string model_name = treeRequest.get("name");
	orchestrator::OrchestratorBase* orchestrator_base = orchestrator::OrchestratorFactory::getInstance()->
		getOrchestrator(model_name);
	vector<orchestrator::TaskInfo> task_infos = orchestrator_base->getRunningTasks();
	std::stringstream response;
	for (orchestrator::TaskInfo task : task_infos)
	{
		if (! task.isActive || ! task.task)
		{
			treeResponse.Add("Status process", "Error initialization of task");
			continue;
		}
		if (task.task->joinable())
		{
			LOG(INFO) << "Json response running the execution : " << response.str() << std::endl;
			if (wait_ms <= 0)
			{
				LOG(INFO) << "Wait infinitely response from task " << std::endl;
				task.task->join();
			}
			else
			{
				if (!task.task->try_join_for(boost::chrono::milliseconds(wait_ms)))
				{
					LOG(WARNING) << "Wait execution timeout after " << wait_ms << "ms. Kill the task now" << std::endl;
					orchestrator_base->killall();
					treeResponse.Add("Status process", "Timeout: process never returned");
					return;
				}
			}
			LOG(INFO) << "Process has ended after " << wait_ms << "ms. return response now" << std::endl;
		}
	}

	treeResponse.write_json(response);
	LOG(INFO) << "Json response running the execution : " << response.str() << std::endl;
}

void executeFromSharedMemory(const string& shm_name)
{
	ChildProcess child;

	child.startProcess([&](json::JsonTree treeRequest) -> json::JsonTree
	{
		json::JsonTree treeResponse = executeJson(treeRequest);

		return treeResponse;
	});
	LOG(INFO) << "Exit from Child now...";
}

void startWebServerProcess(const hipe_engine::ConfigurationChild& config)
{
	std::vector<std::string> args;
	args.push_back("-m");
	args.push_back(corefilter::getLocalEnv().getValue("modulePath"));
	args.push_back("-d");
	args.push_back("-p");
	args.push_back("9090");

	std::string bin_server = "hipe_server";
#ifndef WIN32
	bin_server += ".bin";
#endif

	boost::process::spawn(boost::process::search_path(bin_server), boost::process::args(args));

	int retry = 10;
	while (retry)
	{
		try
		{
			boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, "MySharedMemory");
			break;
		}
		catch (boost::interprocess::interprocess_exception& ex)
		{
			LOG(WARNING) << "Waiting for Http server to start..." << std::endl;
		}
		retry--;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	if (retry <= 0)
	{
		LOG(ERROR) << "Fail to start Http server Debug mode (option -d) is failing please check log to verify server error message." << std::endl;
		exit(-1);
	}
		
}

int main(int argc, char* argv[])
{
	core::Logger::init(argv[0]);


	auto listPotentialWorkingDir = defaultListWorkingDirectory();
	bool foundWorkingDir = false;
	for (std::string& workingDir : listPotentialWorkingDir)
	{
		if (isDirExist(workingDir))
		{
			foundWorkingDir = true;
			if (!SetCurrentWorkingDir(workingDir))
			{
				LOG(WARNING) << "Found unvailable Dir : " << workingDir << " is unvailable. Try next..." << std::endl;
				LOG(WARNING) << "Fail to set Working directory : " << workingDir << std::endl;
				foundWorkingDir = false;
				continue;
			}

			corefilter::getLocalEnv().setValue("workingdir", GetCurrentWorkingDir());
			LOG(INFO) << "Change working directory to " << GetCurrentWorkingDir();
			break;
		}
	}

	if (!foundWorkingDir)
	{
		std::stringstream error_msg;
		error_msg << "Working Directory not found please set HIPE_HOME";
		error_msg << "Default workingdirectory are : [ ";
		for (std::string& workingDir : listPotentialWorkingDir)
			error_msg << workingDir << "; ";
		error_msg << "] " << std::endl;

		LOG(ERROR) << error_msg.str();
		return -1;
	}
	// Default values configuration file and command line configuration
	hipe_engine::ConfigurationChild config;
	std::string home_path = getEnv("HOME");
	std::string config_path = home_path + "/config.json";

	//For attach only
	/*int wait = 1;
	while (wait > 0)
	{
		std::this_thread::sleep_for(std::chrono::seconds(20));

		wait--;	
	}*/

	try
	{
		LOG(INFO) << "Check if home config file exist : " << config_path;
		isFileExist(config_path);

		LOG(INFO) << "Using Home config file at " << config_path;

		config.setConfigFromFile(config_path);
	}
	catch (std::invalid_argument& e)
	{
		LOG(INFO) << "Using config file at " << corefilter::getLocalEnv().getValue("workingdir") << "/" << config_path;
		config.setConfigFromFile("./config.json");
	}

	if (config.setConfigFromCommandLine(argc, argv) == 1)
		return 0;

	config.displayConfig();

	if (config.configuration.aschildproc.empty() && config.configuration.json_request_file.empty())
	{
		LOG(ERROR) <<
			"Need to set a json file as request or a shared memory name as subprocess. For more information ./hipe_engine --help"
			<< std::endl;
		return -1;
	}

	setPythonPath();

	if (config.configuration.debugMode)
	{
		// Run http server from engine and wait for shared memory segment
		startWebServerProcess(config);
	}
	else
	{
		#ifdef WIN32
			SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT | SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
		#endif
	}

	LOG(INFO) << "Hello Hipe";
	LOG(INFO) << "Version : " << getVersion();

	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	std::stringstream buildstring;

	corefilter::getLocalEnv().setValue("base_cert", config.configuration.base_cert);

	std::string currentDir = GetCurrentWorkingDir();
	std::string certDir = currentDir + "/http-root/certificats";
	std::string base_cert = corefilter::getLocalEnv().getValue("base_cert");

	net::log::ForwardLogToWeb* log_service = startLogService();
	LOG(INFO) << "Start Web Log Service on port " << log_service->get_port() << std::endl;
	LOG(INFO) << "Start Orchestrator " << std::endl;
	orchestrator::OrchestratorFactory::start_orchestrator();

	std::shared_ptr<core::ModuleLoader> module = std::make_shared<core::ModuleLoader>(config.configuration.modulePath);
	if (!config.configuration.modulePath.empty())
	{
		try
		{
			module->loadLibrary();
			function<void()> call_function = module->callFunction<void()>("load");
		}
		catch (HipeException& except_loader)
		{
			LOG(ERROR) << "FAIL TO LOAD MODULE AT START TIME. Continue with no preloaded module" << std::endl;
		}
	}
	else
	{
		LOG(WARNING) << "No option to preload module library" << std::endl;
	}
	corefilter::getLocalEnv().setValue("isEngine", "yes");

	LOG(INFO) << "Ready to execute request";
	if (!config.configuration.json_request_file.empty())
	{
		try
		{
			executeFromJsonFile(config.configuration.json_request_file, -1);
		}
		catch (HipeException& ex)
		{
			LOG(ERROR) << "uncatch HipeException : " << ex.what() << std::endl;
			google::FlushLogFiles(google::GLOG_INFO);
			google::FlushLogFiles(google::GLOG_ERROR);
			google::FlushLogFiles(google::GLOG_WARNING);
			return -1;
		}
	}
	else if (!config.configuration.aschildproc.empty())
	{
		try
		{
			executeFromSharedMemory(config.configuration.aschildproc);
		}
		catch (HipeException& ex)
		{
			LOG(ERROR) << "uncatch HipeException : " << ex.what() << std::endl;
			google::FlushLogFiles(google::GLOG_INFO);
			google::FlushLogFiles(google::GLOG_ERROR);
			google::FlushLogFiles(google::GLOG_WARNING);
			return -1;
		}
	}

	google::FlushLogFiles(google::GLOG_INFO);
	google::FlushLogFiles(google::GLOG_ERROR);
	google::FlushLogFiles(google::GLOG_WARNING);

	return 0;
}
