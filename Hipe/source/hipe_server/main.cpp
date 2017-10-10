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
#include "core/version.h"

#include <hipe_server/Configuration.h>

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
	http::HttpServer server(config.configuration.port, 1);

	orchestrator::OrchestratorFactory::start_orchestrator();

	std::thread thread;
	int port = http::start_http_server(config.configuration.port, server, thread);

	//Add resources using path-regex and method-string, and an anonymous function
	//POST-example for the path /string, responds the posted string
	//server.resource["^/string$"]["POST"] = [](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> request) {
	//	//Retrieve string:
	//	auto content = request->content.string();
	//	//request->content.string() is a convenience function for:
	//	//stringstream ss;
	//	//ss << request->content.rdbuf();
	//	//string content=ss.str();

	//	*response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
	//};

	//POST-example for the path /json, responds firstName+" "+lastName from the posted json
	//Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
	//Example posted json:
	//{
	//  "firstName": "John",
	//  "lastName": "Smith",
	//  "age": 25
	//}
	/*server.resource["^/json$"]["POST"] = [](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> request) {
		try {
			ptree pt;
			read_json(request->content, pt);

			ptree pairs = pt.get_child("filter");
			string name = pt.get<string>("firstName") + " " + pt.get<string>("lastName") + " " + pairs.get<string>("name");


			*response << "HTTP/1.1 200 OK\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << name.length() << "\r\n\r\n"
				<< name;
		}
		catch (exception& e) {
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
		}
	};*/

	//GET-example for the path /info
	//Responds with request-information
	//server.resource["^/info$"]["GET"] = [](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> request) {
	//	stringstream content_stream;
	//	content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
	//	content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
	//	for (auto& header : request->header) {
	//		content_stream << header.first << ": " << header.second << "<br>";
	//	}

	//	//find length of content_stream (length received using content_stream.tellp())
	//	content_stream.seekp(0, ios::end);

	//	*response << "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n" << content_stream.rdbuf();
	//};

	////GET-example for the path /match/[number], responds with the matched string in path (number)
	////For instance a request GET /match/123 will receive: 123
	//server.resource["^/match/([0-9]+)$"]["GET"] = [&server](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> request) {
	//	string number = request->path_match[1];
	//	*response << "HTTP/1.1 200 OK\r\nContent-Length: " << number.length() << "\r\n\r\n" << number;
	//};

	////Get example simulating heavy work in a separate thread
	//server.resource["^/work$"]["GET"] = [&server](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> /*request*/) {
	//	thread work_thread([response] {
	//		this_thread::sleep_for(chrono::seconds(5));
	//		string message = "Work done";
	//		*response << "HTTP/1.1 200 OK\r\nContent-Length: " << message.length() << "\r\n\r\n" << message;
	//	});
	//	work_thread.detach();
	//};

	////Default GET-example. If no other matches, this anonymous function will be called. 
	////Will respond with content in the web/-directory, and its subdirectories.
	////Default file: index.html
	////Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
	//server.default_resource["GET"] = [&server](shared_ptr<http::Response<http::HTTP>> response, shared_ptr<http::Request<http::HTTP>> request) {
	//	try {
	//		auto web_root_path = boost::filesystem::canonical("web");
	//		auto path = boost::filesystem::canonical(web_root_path / request->path);
	//		//Check if path is within web_root_path
	//		if (distance(web_root_path.begin(), web_root_path.end())>distance(path.begin(), path.end()) ||
	//			!equal(web_root_path.begin(), web_root_path.end(), path.begin()))
	//			throw invalid_argument("path must be within root path");
	//		if (boost::filesystem::is_directory(path))
	//			path /= "index.html";
	//		if (!(boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)))
	//			throw invalid_argument("file does not exist");

	//		auto ifs = make_shared<ifstream>();
	//		ifs->open(path.string(), ifstream::in | ios::binary);

	//		if (*ifs) {
	//			ifs->seekg(0, ios::end);
	//			auto length = ifs->tellg();

	//			ifs->seekg(0, ios::beg);

	//			*response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n";
	//			default_resource_send(server, response, ifs);
	//		}
	//		else
	//			throw invalid_argument("could not read file");
	//	}
	//	catch (const exception &e) {
	//		string content = "Could not open path " + request->path + ": " + e.what();
	//		*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
	//	}
	//};

	//thread server_thread([&server](){
	//	//Start server
	//	server.start();
	//});

	////Wait for server to start so that the client can connect
	//this_thread::sleep_for(chrono::seconds(1));

	////Client examples
	//HttpClient client("localhost:8080");
	//auto r1 = client.request("GET", "/match/123");
	//cout << r1->content.rdbuf() << endl;
	//cout << "==== End of Match request ==== " << endl;

	//string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25, \"filter\": { \"name\": \"Smoothing\" } }";
	//auto r2 = client.request("POST", "/string", json_string);
	//cout << r2->content.rdbuf() << endl;
	//cout << "==== End of String request ==== " << endl;

	//auto r3 = client.request("POST", "/json", json_string);
	//cout << r3->content.rdbuf() << endl;
	//cout << "==== End of Json Post request ==== " << endl;

	//server_thread.join();
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
