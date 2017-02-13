#include <HttpTask.h>
#include <HttpServer.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <filter/tools/RegisterTable.h>
#include <filter/References.h>
#include <json/JsonBuilder.h>
#include <orchestrator/Orchestrator.h>
#include <filter/data/Composer.h>
#include <core/HipeException.h>


using namespace boost::property_tree;
using namespace std;

core::Logger http::HttpTask::logger = core::setClassNameAttribute("HttpTask");

void http::HttpTask::runTask()
{
	try {
		std::stringstream dataResponse;

		
		ptree treeRequest;
		ptree treeResponse;
		ptree treeResponseInfo;

		read_json(_request->content, treeRequest);

		HttpTask::logger << "Check if algorithm need to be built";
		auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
		treeResponseInfo.add("Algorithm", dataResponse.str());
		dataResponse.str(std::string());

		HttpTask::logger << "Check if orchestrator need to be built";
		auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
		treeResponseInfo.add("Orchestrator", dataResponse.str());
		dataResponse.str(std::string());

		HttpTask::logger << "Bind algorithm " << json_filter_tree->getName() << " to orchestrator " << orchestrator;

		orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
		treeResponseInfo.add("Binding", "OK");
		treeResponse.add_child("Status", treeResponseInfo);

		std::stringstream status;
		write_json(status, treeResponseInfo);
		HttpTask::logger << "Response info :\r\n" << status.str();

		//Check if data is present
		if (treeRequest.count("data") != 0)
		{
			filter::data::IOData data = filter::data::Composer::getDataFromComposer(treeRequest.get_child("data"));

			//Start processing Algorithm with data
			filter::data::OutputData outputData;

			orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

			//after the process execution Data should be OutputData type
			
			
			treeResponse.add_child("dataResponse", outputData.resultAsJson());
		}
		write_json(dataResponse, treeResponse);
		

		*_response << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: application/json\r\n"
			<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
			<< dataResponse.str();
		HttpTask::logger << "HttpTask response has been sent";
	}
	catch (std::exception& e) {
		*_response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
	}

	catch (HipeException& e) {
		*_response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
	}
}
