//@HIPE_LICENSE@
//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
//Added for the default_resource example
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <vector>
#include <algorithm>
#include <http/Server.h>
#include <http/HttpServer.h>
#include <http/HttpClient.h>
#include <orchestrator/Orchestrator.h>
#include <core/Logger.h>
#include <core/version.h>
#include <core/ModuleLoader.h>
#include <glog/log_severity.h>

#include <hipe_server/Configuration.h>
#include <corefilter/tools/Localenv.h>
#include "http/HttpTask.h"
#include "http/HttpsServer.h"
#include "corefilter/tools/net/ForwardLogToWeb.h"

using namespace std;
//Added for the json-example:


typedef http::Client<http::HTTP> HttpClient;

//Added for the default_resource example
void default_resource_send(const http::HttpServer &server, const shared_ptr<http::Response<http::HTTP>> &response,
	const shared_ptr<ifstream> &ifs);

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

net::log::ForwardLogToWeb * startLogService()
{
	net::log::ForwardLogToWeb *logForward = new net::log::ForwardLogToWeb();
	logForward->setName(std::string("Base_log_forward"));
	logForward->setLevel(0);
	int port = 9134;
	logForward->set_port(port);
	logForward->onLoad(nullptr);
	
	

	return logForward;
}

int main(int argc, char* argv[]) {
	core::Logger::init(argv[0]);
	

	

	auto listPotentialWorkingDir = defaultListWorkingDirectory();
	bool foundWorkingDir = false;
	for(std::string & workingDir : listPotentialWorkingDir)
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
		for (std::string & workingDir : listPotentialWorkingDir)
			error_msg << workingDir << "; ";
		error_msg << "] " << std::endl;

		LOG(ERROR) << error_msg.str();
		return -1;
	}
	// Default values configuration file and command line configuration
	hipe_server::Configuration config;
	std::string home_path = getEnv("HOME");
	std::string config_path = home_path + "/config.json";
	
	try
	{
		LOG(INFO) << "Check if home config file exist : " << config_path;
		isFileExist(config_path);

		LOG(INFO) << "Using Home config file at " << config_path;

		config.setConfigFromFile(config_path);
	}
	catch (std::invalid_argument &e)
	{
		LOG(INFO) << "Using config file at " << corefilter::getLocalEnv().getValue("workingdir") << "/" << config_path;
		config.setConfigFromFile("./config.json");
	}

	if (config.setConfigFromCommandLine(argc, argv) == 1)
		return 0;
	
	config.displayConfig();

	setPythonPath();

	LOG(INFO) << "Hello Hipe";
	LOG(INFO) << "Version : " << getVersion();

	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	std::stringstream buildstring;
	buildstring << config.configuration.port;

	corefilter::getLocalEnv().setValue("http_port", buildstring.str());
	corefilter::getLocalEnv().setValue("base_cert",  config.configuration.base_cert);
	
	http::HttpServer server(config.configuration.port, 1);

	std::string currentDir = GetCurrentWorkingDir();
	std::string certDir = currentDir + "/http-root/certificats";
	std::string base_cert = corefilter::getLocalEnv().getValue("base_cert");
	http::HttpsServer server_https(config.configuration.port + 43, 1, base_cert + ".crt", base_cert + ".key");

	net::log::ForwardLogToWeb* log_service = startLogService();
	LOG(INFO) << "Start Web Log Service on port " << log_service->get_port() << std::endl;
	LOG(INFO) << "Start Https service on port " << config.configuration.port + 43 << std::endl;
	LOG(INFO) << "Start Orchestrator " << std::endl;
	orchestrator::OrchestratorFactory::start_orchestrator();

	getProcessController();
	std::thread thread;
	std::thread thread_https;
	http::start_http_server(config.configuration.port, server, thread);
	http::start_https_server(config.configuration.port + 43, server_https, thread_https);

	std::shared_ptr<core::ModuleLoader> module = std::make_shared<core::ModuleLoader>(config.configuration.modulePath);
	if (!config.configuration.modulePath.empty())
	{
		try
		{
			module->loadLibrary();
			function<void()> call_function = module->callFunction<void()>("load");
		}
		catch (HipeException & except_loader)
		{
			std::cerr << "FAIL TO LOAD MODULE AT START TIME. Continue with no preloaded module" << std::endl;
		}
	}
	else
	{
		LOG(INFO) << "No option to preload module library" << std::endl;
	}
	google::FlushLogFiles(google::GLOG_INFO);
	thread.join();

	return 0;
}

void default_resource_send(const http::HttpServer &server, const shared_ptr<http::Response<http::HTTP>> &response,
	const shared_ptr<ifstream> &ifs) {
	//read and send 128 KB at a time
	static vector<char> buffer(131072); // Safe when server is running on one thread
	streamsize read_length;
	if ((read_length = ifs->read(&buffer[0], buffer.size()).gcount()) > 0) {
		response->write(&buffer[0], read_length);
		if (read_length == static_cast<streamsize>(buffer.size())) {
			server.send(response, [&server, response, ifs](const boost::system::error_code &ec) {
				if (!ec)
					default_resource_send(server, response, ifs);
				else
					cerr << "Connection interrupted" << endl;
			});
		}
	}
}
