//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//Added for the default_resource example
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>
#include <http/Server.h>
#include <http/HttpServer.h>
#include <http/HttpClient.h>
#include <orchestrator/Orchestrator.h>
#include <core/Logger.h>

using namespace std;
//Added for the json-example:

typedef http::Client<http::HTTP> HttpClient;


//Added for the default_resource example
void default_resource_send(const http::HttpServer &server, const shared_ptr<http::Response<http::HTTP>> &response,
	const shared_ptr<ifstream> &ifs);

int callMethod(const HttpClient& client, int argc, const char** argv)
{
	if (argc <= 1) return -1;


	boost::filesystem::path p(argv[1]);   // p reads clearer than argv[1] in the following code

	if (boost::filesystem::exists(p))    // does p actually exist?
	{
		if (boost::filesystem::is_regular_file(p))        // is p a regular file?   
			cout << p << " size is " << boost::filesystem::file_size(p) << '\n';

		else if (boost::filesystem::is_directory(p))      // is p a directory?
			cout << p << "is a directory\n";

		else
			cout << p << "exists, but is neither a regular file nor a directory\n";
	}
	else
		cout << p << "does not exist\n";

	return 0;
}

int main(int argc, const char **argv) {
	core::Logger::init();

	core::Logger llogger;

	llogger << core::Logger::Level::info << "Hello Hipe";


	//HTTP-server at port 8080 using 1 thread
	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	//HttpServer server(8080, 1);
	http::HttpServer server(8080, 1);

	orchestrator::OrchestratorFactory::start_orchestrator();

	std::thread thread;
	int port = http::start_http_server(8080, server, thread);


	HttpClient client("localhost:8080");

	callMethod(client, argc, argv);

	string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25, \"filter\": { \"name\": \"Smoothing\" } }";
	auto r3 = client.request("POST", "/json", json_string);
	cout << r3->content.rdbuf() << endl;
	cout << "==== End of Json Post request ==== " << endl;


	thread.join();
	return 0;
}
