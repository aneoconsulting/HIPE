//%HIPE_LICENSE%
#include <ChildController.h>

#include <boost/property_tree/ptree.hpp>

void ChildProcess::startProcess(std::function<json::JsonTree(json::JsonTree treeRequest)> tasker)
{
	copyLocalEnv();
	isActive = true;

	std::thread task = std::thread([&]()
	{
		try
		{
			while (isActive)
			{
				mtx->lock();
				/*LOG(INFO) << "Child : mutex lock" << std::endl;*/
				//Get request first
				
				std::stringstream buffer;
				buffer << shRequest->c_str();
				
				if (buffer.str().empty())
				{
					mtx->unlock();
					/*LOG(INFO) << "Child : mutex unlock" << std::endl;*/
					std::this_thread::sleep_for(std::chrono::milliseconds(200));

					continue;
				}
				std::string request = shRequest->c_str();
				shRequest->assign("");
				
				//Set string to empty to signal the parent that the request has been catch
				json::JsonTree treeRequest;
				treeRequest.read_json(buffer);

				json::JsonTree treeResponse = tasker(treeRequest);
				int errCode = treeResponse.get<int>("errCode");
				

				buffer.str("");
				treeResponse.write_json(buffer);
				std::string jsonResponse = buffer.str();
				shResponse->assign(jsonResponse.c_str());

				mtx->unlock();
				/*LOG(INFO) << "Child : mutex unlock" << std::endl;*/

				if (errCode != 200 && corefilter::getLocalEnv().getValue("debugMode") != "true")
					throw HipeException("Need a new process to avoid bad context and previous error");
			}
			LOG(INFO) << "Exit from hipe engine task";
		}
		catch (HipeException& e)
		{
			mtx->unlock();
			/*LOG(INFO) << "Child : mutex unlock" << std::endl;*/
			LOG(WARNING) << "Exception caught in the process. " << e.what() << std::endl;
			isActive = false;
			if (corefilter::getLocalEnv().getValue("debugMode") != "true")
				exit(-1);
			else
				LOG(WARNING) << "No Restart since hipe_engine is in debug mode" << std::endl;
		}
		catch (std::exception& e)
		{
			mtx->unlock();
			/*LOG(INFO) << "Child : mutex unlock" << std::endl;*/
			LOG(ERROR) << "Something went wrong during hipe_engine execution. std::exception error : " << e.what() <<
				std::endl;
			isActive = false;
			if (corefilter::getLocalEnv().getValue("debugMode") != "true")
				exit(-1);
			else
				LOG(WARNING) << "No Restart since hipe_engine is in debug mode" << std::endl;
		}
	});

	task.join();
	LOG(INFO) << "Parent of process stop" << std::endl;
}
