#include <HttpTask.h>
#include <HttpServer.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <filter/tools/RegisterTable.h>
#include <filter/References.h>
#include <json/JsonBuilder.h>


using namespace boost::property_tree;
using namespace std;

void http::HttpTask::runTask()
{
	try {
		std::string dataResponse = "";

		json::JsonBuilder::buildAlgorithm(dataResponse, _request->content);

		*_response << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: application/json\r\n"
			<< "Content-Length: " << dataResponse.length() << "\r\n\r\n"
			<< dataResponse;
	}
	catch (std::exception& e) {
		*_response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
	}
}
