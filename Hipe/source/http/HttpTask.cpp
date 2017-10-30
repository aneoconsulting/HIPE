#include <HttpTask.h>
#include <HttpServer.h>
#include <orchestrator/Orchestrator.h>
#include <orchestrator/Composer.h>
#include <core/HipeException.h>
#include <http/CommandManager.h>
#include <core/version.h>
#include "core/Localenv.h"
#include <json/JsonBuilder.h>
#ifdef USE_GPERFTOOLS
#include <gperftools/heap-checker.h>
#include <assert.h>
#endif

using namespace std;

core::Logger http::HttpTask::logger = core::setClassNameAttribute("HttpTask");

std::function<bool(std::string, json::JsonTree *)> kill_command() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		if (optionName.compare("Kill") == 0) {
			orchestrator::OrchestratorFactory::getInstance()->killall();
			lptree->Add("Status", "Task has been killed");
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree *)>  exit_command() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		const std::string exit = "Exit";
		if (exit.find(optionName) == 0)
		{
			orchestrator::OrchestratorFactory::getInstance()->killall();
			lptree->Add("Status", "Task has been killed");
			lptree->Add("process", "Server is exiting");
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree *)> get_filters() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		const std::string filters = "Filters";
		int i = 0;
		if (filters.find(optionName) == 0)
		{
			RegisterTable& reg = RegisterTable::getInstance();
			for (auto &name : reg.getTypeNames())
			{
				json::JsonTree parameters;
				json::JsonTree child;
				for (auto &varName : reg.getVarNames(name))
				{
					child.put(varName, "");
				}

				parameters.push_back("", child);
				lptree->AddChild(name, parameters);
				++i;
			}
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree *)> get_version() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		const std::string version = "Version";
		if (version.find(optionName) == 0)
		{
			auto v = getVersion();
			lptree->Add("Version", v);

			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree *)> get_versionHashed() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		const std::string version = "Hash";
		if (version.find(optionName) == 0)
		{
			auto v = getVersionHashed();
			lptree->Add("Hash", v);
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree *)> get_commands_help() {

	return [](std::string OptionName, json::JsonTree * lptree)
	{
		const std::string help = "Help";
		if (help.find(OptionName) == 0) {
			lptree->Add("Version", " returns the running app version number");
			lptree->Add("Hash",    " returns the running app hashed version number ");
			lptree->Add("Exit",    " stop the request");
			lptree->Add("Kill",    " kills the current request");
			lptree->Add("Filters", " get all existing filters in the current version");
			return true;
		}
		return false;
	};
}

void http::HttpTask::runTask() const
{
#ifdef USE_GPERFTOOLS
	static int iteration_leak = 0;
	HeapLeakChecker heap_checker("HttpTask");
#endif
	{
		try {
			stringstream dataResponse;
			json::JsonTree treeRequest;
			json::JsonTree treeResponse;
			json::JsonTree treeResponseInfo;
			treeRequest.read_json(_request->content);
			//read_json(_request->content, treeRequest.get_json_ptree());
			if (treeRequest.count("command") != 0)
			{
				auto command = treeRequest.get_child("command").get("type");
				json::JsonTree  ltreeResponse;// = new JsonTree;
				auto commandFound = CommandManager::callOption(command, get_version(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, kill_command(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_filters(),& ltreeResponse);
				commandFound |= CommandManager::callOption(command, exit_command(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_versionHashed(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_commands_help(),& ltreeResponse);
				if(!commandFound)
				{
					logger << "command "<< command <<" not found", command;

					ltreeResponse.Add(command, " command not found");
				}
				stringstream ldataResponse;
				write_json(ldataResponse, ltreeResponse.get_json_ptree());
				*_response << "HTTP/1.1 200 OK\r\n"
					<< "Access-Control-Allow-Origin: *\r\n"
					<< "Content-Type: application/json\r\n"
					<< "Content-Length: " << ldataResponse.str().length() << "\r\n\r\n"
					<< ldataResponse.str();
				logger << "HttpTask response has been sent";
				HttpTask::logger << ldataResponse.str();
				if (command.find("exit") != std::string::npos)
				{
					exit(0);
				}

				return;
			}


			HttpTask::logger << "Check if algorithm need to be built";
			HttpTask::logger << "Port To listen task was " << core::getLocalEnv().getValue("http_port");

			auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
			const string name = json_filter_tree->getName();
			orchestrator::OrchestratorFactory::getInstance()->addModel(name, json_filter_tree);
			
			json_filter_tree = static_cast<json::JsonFilterTree *>(orchestrator::OrchestratorFactory::getInstance()->getModel(name));
			if (json_filter_tree == nullptr)
				throw HipeException("fail to build or get the model");
			
			treeResponseInfo.Add("Algorithm", dataResponse.str());
			dataResponse.str(std::string());

			HttpTask::logger << "Check if orchestrator need to be built";
			auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);//core::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest->get_json_ptree());
			treeResponseInfo.Add("Orchestrator", dataResponse.str());
			dataResponse.str(std::string());

			stringstream strlog;
			strlog << "Bind algorithm ";
			strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

			HttpTask::logger << strlog.str();

			orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
			treeResponseInfo.Add("Binding", "OK");
			treeResponse.AddChild("Status", treeResponseInfo);

			std::stringstream status;
			write_json(status, treeResponseInfo.get_json_ptree());
			HttpTask::logger << "Response info :\n" << status.str();

			//Check if data is present
			if (treeRequest.count("data") != 0)
			{
				
				auto data = orchestrator::Composer::getDataFromComposer(treeRequest.get_child("data"));
				//Start processing Algorithm with data
				filter::data::Data outputData;

				orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

				//after the process execution Data should be an OutputData type
				if (outputData.getType() == filter::data::IMGB64)
				{
					filter::data::OutputData output_data;
					output_data = outputData;
					auto outpd = output_data.resultAsJson();

					treeResponse.AddChild("dataResponse",outpd);
				}
			}
			
			treeResponse.write_json(dataResponse);

			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
			HttpTask::logger << "HttpTask response has been sent";
			HttpTask::logger << dataResponse.str();
		}
		catch (std::exception& e) {
			json::JsonTree treeResponse;
			treeResponse.Add("Status", e.what());
			std::stringstream dataResponse;
			treeResponse.write_json(dataResponse);
			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
		}

		catch (HipeException& e) {
			json::JsonTree treeResponse;
			treeResponse.Add("Status", e.what());
			std::stringstream dataResponse;
			treeResponse.write_json(dataResponse);
			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
		}
	}
#ifdef USE_GPERFTOOLS
	iteration_leak++;
	//if (iteration_leak == 10)
	{
		if (!heap_checker.NoLeaks()) assert(NULL == "heap memory leak");
	}

#endif

}
