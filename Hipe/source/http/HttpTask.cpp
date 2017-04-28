#include <HttpTask.h>
#include <HttpServer.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
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
		HttpTask::logger << "Response info :\n" << status.str();

		//Check if data is present
		if (treeRequest.count("data") != 0)
		{
			shared_ptr<filter::data::IOData> data = filter::data::Composer::getDataFromComposer(treeRequest.get_child("data"));
			
			if (data.get()->getType() == filter::data::IODataType::LISTIO)
			{
				filter::data::ListIOData & list_io_data = static_cast<filter::data::ListIOData&>(*data.get());
			}

			//Start processing Algorithm with data
			shared_ptr<filter::data::IOData> outputData;

			orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

			//after the process execution Data should be an OutputData type
			filter::data::OutputData * output_data = static_cast<filter::data::OutputData *>(outputData.get());
			
			if (outputData.get() != nullptr)
				treeResponse.add_child("dataResponse", output_data->resultAsJson());
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
