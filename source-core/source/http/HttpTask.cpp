//@HIPE_LICENSE@

#include <string>
#include <core/misc.h>


#include <HttpTask.h>
#include <HttpServer.h>
#include <corefilter/tools/JsonBuilder.h>
#include <orchestrator/Orchestrator.h>
#include <orchestrator/Composer.h>
#include <core/HipeException.h>
#include <corefilter/tools/CommandManager.h>
#include <core/version.h>
#include <corefilter/tools/Localenv.h>

#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>
#include <functional>
#include <utility>
#include <fstream>

#pragma warning(pop)

#ifdef USE_GPERFTOOLS
#include <gperftools/heap-checker.h>
#include <assert.h>
#endif


#include "ProcessController.h"


using namespace std;


void http::HttpTask::run_process()
{
	stringstream dataResponse;
	std::string request = _request->content.string();

	process = getProcessController();

	if (corefilter::getLocalEnv().getValue("debugMode") == "true")
	{
		LOG(INFO) << "Running in debugMode session will wait for hipe_engine" << std::endl;

		dataResponse << process->executeOrUpdateAttachedProcess(request);	
	}
	else
	{
		LOG(INFO) << "Starting hipe_engine by the server itself" << std::endl;
		dataResponse << process->executeOrUpdateProcess(request);	
	}
	
	

	string jsonResponse = dataResponse.str();

	*_response << "HTTP/1.1 200 OK\r\n"
		<< "Access-Control-Allow-Origin: *\r\n"
		<< "Content-Type: application/json\r\n"
		<< "Content-Length: " << jsonResponse.length() << "\r\n\r\n"
		<< jsonResponse;
	LOG(INFO) << "HttpTask response has been sent";
	VLOG(3) << jsonResponse;

}

void http::HttpTask::readFileContent(const std::string& local_path, std::stringstream& data_response)
{
	ifstream myfile;
	myfile.open(local_path, std::ifstream::binary | std::ifstream::in);
	if (myfile.is_open())
	{
		std::copy(
			std::istreambuf_iterator<char>(myfile),
			(std::istreambuf_iterator<char>()),
			std::ostreambuf_iterator<char>(data_response));
		myfile.close();
	}
}

void http::HttpTask::RenderHtml() const
{
	try
	{
		//Move current directory to root dir of WorkingDir
		std::string workingRootDir = corefilter::getLocalEnv().getValue("workingdir")	;
		stringstream dataResponse;
		std::string extension = "html";
		std::string currentDir;
		std::string oldDir = GetCurrentWorkingDir();
		std::string wwwDir = workingRootDir + "/http-root/www";
		if (isDirExist(wwwDir))
		{
			if (!SetCurrentWorkingDir(wwwDir))
			{
				LOG(ERROR) << "Fail to set " << wwwDir << " for HTML rendering" << std::endl;
			}
		}
		currentDir = GetCurrentWorkingDir();
		LOG(INFO) << "Request ressources " << _request->path.c_str();

		if (_request->path == "/demos.html" || _request->path == "/")
		{
			extension = "text/html";
			string local_path = currentDir + "/index.html";
			readFileContent(local_path, dataResponse);
		}
		else
		{
			string local_path = currentDir + _request->path;

			if (_request->path.find("/js/") == 0)
			{
				extension = "text/javascript";
				readFileContent(local_path, dataResponse);
			}
			else if(_request->path.find("/css/") == 0)
			{
				extension = "text/css";
				readFileContent(local_path, dataResponse);
			}
			else if (_request->path.find("/images/") == 0)
			{
				readFileContent(local_path, dataResponse);
				std::string data_image = dataResponse.str();
				const unsigned char* data = (const unsigned char *)(data_image.c_str());
				extension = std::string("image/") + getExtensionFromImageData(data, dataResponse.str().size());
			}
			else if (_request->path == "/index.html")
			{
				extension = "text/html";
				string local_path = currentDir + "/index.html";
				readFileContent(local_path, dataResponse);
			}
			else if (_request->path.find(".html") != std::string::npos)
			{
				extension = "text/html";
				string local_path = currentDir + _request->path;
				readFileContent(local_path, dataResponse);
			}
			else if (_request->path.find("/demos/") == 0 && _request->path.find(".json") != std::string::npos)
			{
				extension = "application/json";
				string local_path = currentDir + _request->path;
				readFileContent(local_path, dataResponse);
			}
			else if (_request->path.find(".ico") != std::string::npos)
			{
			
				string local_path = currentDir + _request->path;
				readFileContent(local_path, dataResponse);
				const unsigned char* data = (const unsigned char *)(dataResponse.str().c_str());
				extension = std::string("image/x-") + getExtensionFromImageData(data, dataResponse.str().size());
			}
		}
		if (!SetCurrentWorkingDir(oldDir))
		{
			LOG(ERROR) << "Fail to set " << oldDir << " after HTML rendering" << std::endl;
		}

		*_response << "HTTP/1.1 200 OK\r\n"
			<< "Access-Control-Allow-Origin: *\r\n"
			<< "Content-Type: " << extension << "\r\n"
			<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
			<< dataResponse.str();
		LOG(INFO) << "Html page sent ";
		VLOG(3) << dataResponse.str();
	}
	catch (std::exception& e)
	{
		std::string error = e.what();

		*_response << "HTTP/1.1 200 OK\r\n"
			<< "Access-Control-Allow-Origin: *\r\n"
			<< "Content-Type: text/plain" << "\r\n"
			<< "Content-Length: " << error.size() << "\r\n\r\n"
			<< error;
	}
}



void http::HttpTask::runTask() const
{
#ifdef USE_GPERFTOOLS
	static int iteration_leak = 0;
	HeapLeakChecker heap_checker("HttpTask");
#endif
	{
		try
		{
			stringstream dataResponse;
			json::JsonTree treeRequest;
			json::JsonTree treeResponse;
			json::JsonTree treeResponseInfo;
			treeRequest.read_json(_request->content);
			int errorCode = 200;
			std::string stringErrCode = " OK\r\n";

			if (treeRequest.count("command") != 0)
			{
				auto command = treeRequest.get_child("command").get<std::string>("type");
				json::JsonTree ltreeResponse;// = new JsonTree;
				auto commandFound = corefilter::CommandManager::callOption(command, corefilter::get_version(), &ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, orchestrator::kill_command(), &ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_filters(), & ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, orchestrator::exit_command(), &ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_versionHashed(), &ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_commands_help(), & ltreeResponse);
				commandFound |= corefilter::CommandManager::callOption(command, corefilter::get_groupFilter(), &ltreeResponse);

				if (!commandFound)
				{
					LOG(INFO) << "command " << command << " not found" , command;

					ltreeResponse.Add(command, " command not found");
					errorCode = 400;
					stringErrCode = " Bad Request\r\n";
				}
				stringstream ldataResponse;
				ltreeResponse.write_json(ldataResponse);
				*_response << "HTTP/1.1 " << errorCode << stringErrCode
					<< "Access-Control-Allow-Origin: *\r\n"
					<< "Content-Type: application/json\r\n"
					<< "Content-Length: " << ldataResponse.str().length() << "\r\n\r\n"
					<< ldataResponse.str();
				LOG(INFO) << "HttpTask response has been sent";
				VLOG(3) << ldataResponse.str();
				if (command.find("exit") != std::string::npos)
				{
					exit(0);
				}

				return;
			}


			LOG(INFO) << "Check if algorithm need to be built";
			LOG(INFO) << "Port To listen task was " << corefilter::getLocalEnv().getValue("http_port");

			auto json_filter_tree = json::JsonBuilder::buildAlgorithm(dataResponse, treeRequest);
			const string name = json_filter_tree->getName();
			orchestrator::OrchestratorFactory::getInstance()->addModel(name, json_filter_tree);

			json_filter_tree = static_cast<json::JsonFilterTree *>(orchestrator::OrchestratorFactory::getInstance()->getModel(name));
			if (json_filter_tree == nullptr)
				throw HipeException("fail to build or get the model");

			treeResponseInfo.Add("Algorithm", dataResponse.str());
			dataResponse.str(std::string());

			LOG(INFO) << "Check if orchestrator need to be built";
			auto orchestrator = json::JsonBuilder::getOrBuildOrchestrator(dataResponse, treeRequest);
			treeResponseInfo.Add("Orchestrator", dataResponse.str());
			dataResponse.str(std::string());

			stringstream strlog;
			strlog << "Bind algorithm ";
			strlog << json_filter_tree->getName() << " to orchestrator " << orchestrator;

			VLOG(3) << strlog.str();

			orchestrator::OrchestratorFactory::getInstance()->bindModel(json_filter_tree->getName(), orchestrator);
			treeResponseInfo.Add("Binding", "OK");
			treeResponse.add_child("Status", treeResponseInfo);

			std::stringstream status;
			treeResponseInfo.write_json(status);
			VLOG(3) << "Response info :\n" << status.str();

			//Check if data is present
			if (treeRequest.count("data") != 0)
			{
				boost::property_tree::ptree& pt = treeRequest.get_child("data");
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
			LOG(INFO) << "HttpTask response has been sent";
			VLOG(3) << dataResponse.str();
		}
		catch (HipeException& e)
		{
			int errorCode = 400;
			std::string errorMsg = std::string(e.what());

			if (errorMsg.find("Old Exception was") != std::string::npos)
				errorCode = 200;

			json::JsonTree treeResponse;
			treeResponse.Add("Status", e.what());
			std::stringstream dataResponse;
			treeResponse.write_json(dataResponse);
			*_response << "HTTP/1.1 " << errorCode << " OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
			LOG(ERROR) << "HipeException : " << e.what();
			PRINTSTACK(INFO);
		}
		catch (std::exception& e)
		{
			json::JsonTree treeResponse;
			treeResponse.Add("Status", e.what());
			std::stringstream dataResponse;
			treeResponse.write_json(dataResponse);
			*_response << "HTTP/1.1 400 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();
			LOG(ERROR) << "std::exception : " << e.what();
			PRINTSTACK(INFO);
		}
		catch (...)
		{
			json::JsonTree treeResponse;
			treeResponse.Add("Status", "Uknown exception");
			std::stringstream dataResponse;
			treeResponse.write_json(dataResponse);
			*_response << "HTTP/1.1 400 OK\r\n"
				<< "Access-Control-Allow-Origin: *\r\n"
				<< "Content-Type: application/json\r\n"
				<< "Content-Length: " << dataResponse.str().length() << "\r\n\r\n"
				<< dataResponse.str();

			LOG(ERROR) << "Unkown exception in httptask...";
			PRINTSTACK(INFO);
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
