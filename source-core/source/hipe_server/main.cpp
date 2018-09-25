//@HIPE_LICENSE@
//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
//Added for the default_resource example
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

	//If the binary was runned from core binary folder
	relativePath.push_back("../../workingdir");

	//If The binary was runned from one of modules
	relativePath.push_back("../../../workingdir");

	//If The binary was runned from workingDir
	relativePath.push_back("../workingdir");




	return relativePath;
}


int main(int argc, char* argv[]) {
	core::Logger::init(argv[0]);
	

	// Default values configuration file and command line configuration
	hipe_server::Configuration config;
	config.setConfigFromFile("./config.json");
	if (config.setConfigFromCommandLine(argc, argv) == 1)
		return 0;

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
			corefilter::getLocalEnv().setValue("workingdir", workingDir);

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

	
	config.displayConfig();

	LOG(INFO) << "Hello Hipe";
	LOG(INFO) << "Version : " << getVersion();

	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	std::stringstream buildstring;
	buildstring << config.configuration.port;

	corefilter::getLocalEnv().setValue("http_port", buildstring.str());
	
	http::HttpServer server(config.configuration.port, 1);

	std::string currentDir = GetCurrentWorkingDir();
	std::string certDir = currentDir + "/http-root/certificats";
	http::HttpsServer server_https(config.configuration.port + 43, 1, certDir + "/mylaptop.crt", certDir + "/mylaptop.key");

	orchestrator::OrchestratorFactory::start_orchestrator();

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
