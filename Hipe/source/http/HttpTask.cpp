#include <HttpTask.h>
#include <HttpServer.h>
#include <json/JsonBuilder.h>
#include <orchestrator/Orchestrator.h>
#include <orchestrator/Composer.h>
#include <core/HipeException.h>
#include <http/CommandManager.h>
#include <core/version.h>
#include <core/Localenv.h>

#include <boost/property_tree/ptree.hpp>

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
				json::JsonTree info;
				for (auto &varName : reg.getVarNames(name))
				{
					child.put(varName, "");
				}
				info.put("namespace", reg.getNamespace(name));
				parameters.push_back("fields", child);
				parameters.push_back("info", info);
				
				lptree->add_child(name, parameters);
				++i;
			}
			return true;
		}
		return false;
	};
}
std::vector<std::string> splitfilterNamespaces(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

std::map < std::string, std::vector<json::JsonTree>>get_map_filters() {

	std::map<std::string, std::vector<json::JsonTree>> tree;

	int i = 0;
	RegisterTable& reg = RegisterTable::getInstance();
	for (auto &name : reg.getTypeNames())
	{
		auto name_spacce_filter = reg.getNamespace(name);
		json::JsonTree filterNode;

		for (auto &varName : reg.getVarNames(name))
		{
			filterNode.put(varName, "");
		}
		json::JsonTree child;
		child.push_back(name, filterNode);

		tree[name_spacce_filter].push_back( child);
	}
	return tree;

}

	//! \brief new get filters: each path is splitted on directories
//! \todo test when all namepaces will be set => change function name to get_filters() (remove new keyword)
std::function<bool(std::string, json::JsonTree *)> get_groupFilter() {
	return [](std::string optionName, json::JsonTree *lptree)
	{
		const std::string filters = "GroupFilters";
		int i = 0;
		if (filters.find(optionName) == 0)
		{
			auto map_tree = get_map_filters();
			for (auto &mt : map_tree)
			{
				auto keys = splitfilterNamespaces(mt.first, '/');
				auto length = keys.size();
				auto lastElement = keys[length - 1];
				json::JsonTree lastElementJson;
				auto values = mt.second;
				for (auto &v : values)
				{
					lastElementJson.push_back("", v);
				}
				json::JsonTree* elements =  new json::JsonTree[length];
				
				elements[length - 1] = lastElementJson;
				int i = 0;
				for (i = length - 2; i >= 0; i--)
				{
					json::JsonTree element;
					element.add_child(keys[i+1], elements[i + 1]);
					if (lptree->count(keys[i]) == 1) {
						lptree->add_child_to_child(keys[i], keys[i+1], elements[i + 1]);
					}
					else
					{
						lptree->add_child(keys[i + 1], element);
					}
					elements[i] = element;
				}
				if(lptree->count(keys[0]) == 0)
					lptree->add_child(keys[0], elements[0]);
				
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
			lptree->Add("GroupFilters", "get all existing filter in groups");
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
			
			if (treeRequest.count("command") != 0)
			{
				auto command = treeRequest.get_child("command").get<std::string>("type");
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
				ltreeResponse.write_json(ldataResponse);
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
			auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
			treeResponseInfo.Add("Orchestrator", dataResponse.str());
			dataResponse.str(std::string());

			stringstream strlog;
			strlog << "Bind algorithm ";
			strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

			HttpTask::logger << strlog.str();

			orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
			treeResponseInfo.Add("Binding", "OK");
			treeResponse.add_child("Status", treeResponseInfo);

			std::stringstream status;
			treeResponseInfo.write_json(status);
			HttpTask::logger << "Response info :\n" << status.str();

			//Check if data is present
			if (treeRequest.count("data") != 0)
			{
				boost::property_tree::ptree & pt = treeRequest.get_child("data");
				json::JsonTree jtr(pt);
				auto data = orchestrator::Composer::getDataFromComposer(jtr);
				//Start processing Algorithm with data
				data::Data outputData;

				orchestrator::OrchestratorFactory::getInstance()->process(json_filter_tree->getName(), data, outputData);

				//after the process execution Data should be an OutputData type
				if (outputData.getType() == data::IMGB64)
				{
					throw HipeException("Need to rethink the output of a image. Since there is a core data. The child of OutputData should use JsonTree directly in the filter modules");
				/*	data::OutputData output_data;
					output_data = outputData;
					auto outpd = json::JsonBuilder::buildJson(output_data);

					treeResponse.AddChild("dataResponse",outpd);*/
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
