#include <HttpTask.h>
#include <HttpServer.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/JsonBuilder.h>
#include <orchestrator/Orchestrator.h>
#include <filter/data/Composer.h>
#include <core/HipeException.h>
#include <http/CommandManager.h>
#include <core/version.h>
#ifdef USE_GPERFTOOLS
#include <gperftools/heap-checker.h>
#include <assert.h>
#endif


using namespace boost::property_tree;
using namespace std;

core::Logger http::HttpTask::logger = core::setClassNameAttribute("HttpTask");

std::function<bool(std::string, boost::property_tree::ptree *)> kill_command() {
	return [](std::string optionName, boost::property_tree::ptree *lptree)
	{
		if (optionName.compare("kill") == 0) {
			orchestrator::OrchestratorFactory::getInstance()->killall();
			lptree->add("Status", "Task has been killed");
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, boost::property_tree::ptree *)>  exit_command() {
	return [](std::string optionName, boost::property_tree::ptree *lptree)
	{
		const std::string exit = "exit";
		if (exit.find(optionName) == 0)
		{
			orchestrator::OrchestratorFactory::getInstance()->killall();
			lptree->add("Status", "Task has been killed");
			lptree->add("process", "Server is exiting");
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, boost::property_tree::ptree *)> get_filters() {
	return [](std::string optionName, boost::property_tree::ptree *lptree)
	{
		const std::string filters = "filters";
		int i = 0;
		if (filters.find(optionName) == 0)
		{
			RegisterTable & reg = RegisterTable::getInstance();
			for (auto &name : reg.getTypeNames())
			{
				boost::property_tree::ptree parameters;
				boost::property_tree::ptree child;
				for (auto &varName : reg.getVarNames(name))
				{
					child.put(varName, "");
				}

				parameters.push_back(std::make_pair("", child));
				lptree->add_child(name, parameters);
				++i;
			}
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, boost::property_tree::ptree *)> get_version() {
	return [](std::string optionName, boost::property_tree::ptree *lptree)
	{
		const std::string version = "version";
		if (version.find(optionName) == 0)
		{
			auto v = getVersion();
			lptree->add("Version", v);

			return true;
		}
		return false;
	};
}

std::function<bool(std::string, boost::property_tree::ptree *)> get_versionHashed() {
	return [](std::string optionName, boost::property_tree::ptree *lptree)
	{
		const std::string version = "hash";
		if (version.find(optionName) == 0)
		{
			auto v = getVersionHashed();
			lptree->add("hash", v);

			return true;
		}
		return false;
	};
}

std::function<bool(std::string, boost::property_tree::ptree*)> get_commands_help() {

	return [](std::string OptionName, boost::property_tree::ptree* lptree)
	{
		const std::string help = "Help";
		if (help.find(OptionName) == 0) {
			lptree->add("Version", " returns the running app version number");
			lptree->add("Hash",    " returns the running app hashed version number ");
			lptree->add("exit",    " stop the request");
			lptree->add("kill",    " kills the current request");
			lptree->add("filters", " get all existing filters in the current version");
			return true;
		}
		return false;
	};
}

void http::HttpTask::runTask()
{
#ifdef USE_GPERFTOOLS
	static int iteration_leak = 0;
	HeapLeakChecker heap_checker("HttpTask");
#endif
	{
		try {
			std::stringstream dataResponse;

			ptree treeRequest;
			ptree treeResponse;
			ptree treeResponseInfo;

			read_json(_request->content, treeRequest);
			if (treeRequest.count("command") != 0)
			{
				auto command = treeRequest.get_child("command").get<std::string>("type");
				ptree ltreeResponse;

				auto commandFound = CommandManager::callOption(command, get_version(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, kill_command(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_filters(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, exit_command(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_versionHashed(), &ltreeResponse);
				commandFound |= CommandManager::callOption(command, get_commands_help(), &ltreeResponse);
				if(!commandFound)
				{
					ltreeResponse.add(command, " command not found");
				}
				stringstream ldataResponse;
				write_json(ldataResponse, ltreeResponse);
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
			auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
			treeResponseInfo.add("Algorithm", dataResponse.str());
			dataResponse.str(std::string());

			HttpTask::logger << "Check if orchestrator need to be built";
			auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
			treeResponseInfo.add("Orchestrator", dataResponse.str());
			dataResponse.str(std::string());

			stringstream strlog;
			strlog << "Bind algorithm ";
			strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

			HttpTask::logger << strlog.str();

			orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
			treeResponseInfo.add("Binding", "OK");
			treeResponse.add_child("Status", treeResponseInfo);

			std::stringstream status;
			write_json(status, treeResponseInfo);
			HttpTask::logger << "Response info :\n" << status.str();

			//Check if data is present
			if (treeRequest.count("data") != 0)
			{
				filter::data::Data data = filter::data::Composer::getDataFromComposer(treeRequest.get_child("data"));

				if (data.getType() == filter::data::IODataType::LISTIO)
				{
					filter::data::ListIOData & list_io_data = static_cast<filter::data::ListIOData&>(data);
				}

				//Start processing Algorithm with data
				filter::data::Data outputData;

				orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

				//after the process execution Data should be an OutputData type
				if (outputData.getType() == filter::data::IMGB64)
				{
					filter::data::OutputData output_data;
					output_data = outputData;;


					treeResponse.add_child("dataResponse", output_data.resultAsJson());
				}
			}
			write_json(dataResponse, treeResponse);


			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
			HttpTask::logger << "HttpTask response has been sent";
			HttpTask::logger << dataResponse.str();
		}
		catch (std::exception& e) {
			ptree treeResponse;
			treeResponse.add("Status", e.what());
			std::stringstream dataResponse;
			write_json(dataResponse, treeResponse);
			*_response << "HTTP/1.1 200 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
		}

		catch (HipeException& e) {
			ptree treeResponse;
			treeResponse.add("Status", e.what());
			std::stringstream dataResponse;
			write_json(dataResponse, treeResponse);
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
