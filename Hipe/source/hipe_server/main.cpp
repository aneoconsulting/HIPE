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

#include <hipe_server/Configuration.h>
#include "core/Localenv.h"

using namespace std;
//Added for the json-example:


typedef http::Client<http::HTTP> HttpClient;

//Added for the default_resource example
void default_resource_send(const http::HttpServer &server, const shared_ptr<http::Response<http::HTTP>> &response,
	const shared_ptr<ifstream> &ifs);

int main(int argc, char* argv[]) {
	core::Logger::init();
	core::Logger llogger = core::setClassNameAttribute("Main");

	// Default values configuration file and command line configuration
	hipe_server::Configuration config;
	config.setConfigFromFile("./config.json");
	if (config.setConfigFromCommandLine(argc, argv) == 1)
		return 0;

	config.displayConfig();

	llogger << core::Logger::Level::info << "Hello Hipe";
	llogger << core::Logger::Level::info << "Version : " << getVersion();

	//HTTP-server at port 8080 using 1 thread
	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	std::stringstream buildstring;
	buildstring << config.configuration.port;

	core::getLocalEnv().setValue("http_port", buildstring.str());

	http::HttpServer server(config.configuration.port, 1);

	std::shared_ptr<core::ModuleLoader> module = std::make_shared<core::ModuleLoader>(config.configuration.modulePath);
	module->loadLibrary();
	//function<RegisterTable*()> call_function = module->callFunction<RegisterTable*()>("c_registerInstance");
	//RegisterTable * res = call_function();
	orchestrator::OrchestratorFactory::start_orchestrator();

	std::thread thread;
	int port = http::start_http_server(config.configuration.port, server, thread);

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
