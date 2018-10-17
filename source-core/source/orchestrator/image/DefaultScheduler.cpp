//@HIPE_LICENSE@

#include <orchestrator/image/DefaultScheduler.h>
#include <core/FatalException.h>
#include "corefilter/tools/Localenv.h"
#include <glog/logging.h>

orchestrator::image::DefaultScheduler::DefaultScheduler()
{
	std::thread::id root;
	pyMainThreaState[root] = nullptr;
	interp[root] = nullptr;
	pyThreadState[root] = nullptr;


	if (!Py_IsInitialized())
	{
		try
		{
			Py_InitializeEx(0);
			// Create GIL/enable threads
			PyEval_InitThreads();
			pyMainThreaState[root] = PyGILState_GetThisThreadState();

			PyEval_ReleaseThread(pyMainThreaState[root]);
		}
		catch (...)
		{
			if (PyErr_Occurred())
				PyErr_Print();
			throw HipeException("Cannot initialize Python");
		}
	}
}

int orchestrator::image::DefaultScheduler::getMaxLevelNode(filter::Model* filter, int level)
{
	for (auto& child : filter->getChildrens())
	{
		int current = child.second->getLevel();

		if (level < current) level = current;

		current = getMaxLevelNode(child.second, level);
		if (level < current) level = current;
	}
	return level;
}

int orchestrator::image::DefaultScheduler::setMatrixLayer(filter::Model* filter, MatrixLayerNode& matrixLayerNode)
{
	if (std::find(matrixLayerNode[filter->getLevel()].begin(), matrixLayerNode[filter->getLevel()].end(), filter)
		!= matrixLayerNode[filter->getLevel()].end())
		return 0;

	matrixLayerNode[filter->getLevel()].push_back(filter);

	for (auto& children : filter->getChildrens())
	{
		//matrixLayerNode[children.second->getLevel()].push_back(filter);

		setMatrixLayer(children.second, matrixLayerNode);
	}
	return 0;
}

void orchestrator::image::DefaultScheduler::popUnusedData(filter::Model* filter)
{
	data::ConnexDataBase& outRes = filter->getConnector();
	data::DataPort& data_port = static_cast<data::DataPort &>(outRes.getPort());

	while (!data_port.empty())
	{
		data_port.pop();
	}
}

void orchestrator::image::DefaultScheduler::disposeChild(filter::Model* filter)
{
	filter->dispose();

	data::ConnexDataBase& outRes = filter->getConnector();
	data::DataPort& data_port = static_cast<data::DataPort &>(outRes.getPort());

	while (! data_port.empty())
	{
		data_port.pop();
	}

	for (auto& childFilter : filter->getChildrens())
	{
		childFilter.second->dispose();
		disposeChild(childFilter.second);

		data::ConnexDataBase& outResChild = childFilter.second->getConnector();
		data::DataPort& data_portChild = static_cast<data::DataPort &>(outResChild.getPort());

		while (! data_portChild.empty())
		{
			data_portChild.pop();
		}
	}
}

void orchestrator::image::DefaultScheduler::CallFiltersOnload(filter::Model* filter, int level)
{
	//// Get the default thread state  
	std::thread::id root;

	pyMainThreaState[root];


	if (level == 0 && pyThreadState[std::this_thread::get_id()] == nullptr)
	{
		PyEval_AcquireThread(pyMainThreaState[root]);
		pyThreadState[std::this_thread::get_id()] = Py_NewInterpreter();

		PyThreadState_Swap(pyMainThreaState[root]);
		PyEval_ReleaseThread(pyMainThreaState[root]);
	}


	if (filter->isPython())
	{
		PyEval_AcquireThread(pyMainThreaState[root]);
		interp[std::this_thread::get_id()] = pyThreadState[std::this_thread::get_id()]->interp;
		filter->onLoad(interp[std::this_thread::get_id()]);
		PyEval_ReleaseThread(pyMainThreaState[root]);
	}
	else
	{
		filter->onLoad(nullptr);
	}


	for (auto& childFilter : filter->getChildrens())
	{
		CallFiltersOnload(childFilter.second, level + 1);
	}

	if (level == 0)
	{
		pyThreadState[std::this_thread::get_id()] = nullptr;
	}
}

void orchestrator::image::DefaultScheduler::InitNewPythonThread(PyInterpreterState* interpreterPython, PyExternalUser*& pyExternalUser)
{
	if (! pyExternalUser)
	{
		//interp = ts->interp;
		//PyThreadState_Swap(pyThreadState);
		pyExternalUser = new PyExternalUser(interpreterPython);
	}
}

void orchestrator::image::DefaultScheduler::onStartCall(filter::Model* model, void* context)
{
	if (! model->isPython()) // all other than pythonFilter
	{
		model->onStart(context);
	}


	for (auto& childFilter : model->getChildrens())
	{
		onStartCall(childFilter.second, context);
	}
}

void orchestrator::image::DefaultScheduler::setPythonUserThreadState(filter::Model* model, PyExternalUser* py_external_user)
{
	if (model->isPython())
	{
		model->onStart(py_external_user);
	}


	for (auto& childFilter : model->getChildrens())
	{
		setPythonUserThreadState(childFilter.second, py_external_user);
	}
}

void orchestrator::image::DefaultScheduler::destroyPythonThread(PyExternalUser*& pyExternalUser)
{
	if (pyExternalUser)
	{
		delete pyExternalUser;
		pyExternalUser = nullptr;
	}
}

#include <stdlib.h>
#include <signal.h>

void SignalHandler(int signal)
{
	std::stringstream build;
	build << "Fatal Error : Signal " << signal;
	std::cerr << build.str();
	exit(-1);
}


void orchestrator::image::DefaultScheduler::processDataSource(filter::Model* root, data::Data& outputData, bool debug)
{
	TaskInfo taskInfo;
	runningTasks.push_back(taskInfo);
	//Change directory to the root data dire i.e workingdir/root
	std::string rootDir = corefilter::getLocalEnv().getValue("workingdir") + "/root";
	if (isDirExist(rootDir))
	{
		if (!SetCurrentWorkingDir(rootDir))
		{
			LOG(WARNING) << "Found unvailable Dir : " << rootDir << " is unvailable. Try next..." << std::endl;
			LOG(WARNING) << "Fail to set Working directory : " << rootDir << std::endl;
		}
	}

	std::shared_ptr<std::exception_ptr>& texptr = runningTasks[0].texptr;

	filter::Model* filterRoot = root->getRootFilter();

	//Check if the first node is the only one and if the first node is a rootFilter
	const int maxLevel = getMaxLevelNode(filterRoot);

	cleanDataChild(filterRoot);
	MatrixLayerNode matrixLayer(maxLevel + 1);
	setMatrixLayer(filterRoot, matrixLayer);
	//Get the Datasource type. FI : if One of data is a video then we detach the thread

	data::IODataType sourceType = data::IMGF;
	for (auto dataSource : matrixLayer[1])
	{
		filter::datasource::DataSource* data_source = dynamic_cast<filter::datasource::DataSource*>(dataSource);
		if (data_source != nullptr)
		{
			if (data_source->getSourceType() == data::IODataType::VIDF)
			{
				sourceType = data::IODataType::VIDF;
				data_source->intialize();
			}
		}
	}


	filter::Model* cpyFilterRoot = copyAlgorithms(filterRoot);

	try
	{
		CallFiltersOnload(cpyFilterRoot);
	}
	catch (HipeException& e)
	{
		cleanDataChild(cpyFilterRoot);
		disposeChild(cpyFilterRoot);
		if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
			throw HipeException("Cannot free properly Filter graph");
		std::stringstream buildError;
		buildError << "Fail to load Filter caling method CallFilterOnLoad Inner exception\n";
		buildError << e.what();

		throw HipeException(buildError.str());
	}
	std::atomic<bool>* isActive = new std::atomic<bool>(true);
	PyInterpreterState* l_interp = interp[std::this_thread::get_id()];


	boost::thread* task = new boost::thread([cpyFilterRoot, maxLevel, &outputData, sourceType, isActive, l_interp, &texptr]()
	{
		typedef void (*SignalHandlerPointer)(int);

		SignalHandlerPointer previousHandler;
		//previousHandler = signal(SIGSEGV, SignalHandler);

		MatrixLayerNode matrixLayer(maxLevel + 1);

		//TODO insert debug layers into the matrix
		setMatrixLayer(cpyFilterRoot, matrixLayer);
		//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
		//std::shared_ptr<data::Data> inter_output;
		HipeStatus hipe_status = OK;

		//Create new python ThreadState for filter which need it
		PyExternalUser* userThreadState = nullptr;
		if (l_interp != nullptr)
			InitNewPythonThread(l_interp, userThreadState);

		setPythonUserThreadState(cpyFilterRoot, userThreadState);

		//Call init method if need by each filter
		onStartCall(cpyFilterRoot, nullptr);

		while ((hipe_status != END_OF_STREAM) && *isActive)
		{
			for (unsigned int layer = 1; layer < matrixLayer.size() - 1; layer++)
			{
				for (auto& filter : matrixLayer[layer])
				{
					try
					{
						hipe_status = filter->process();
						if (hipe_status == END_OF_STREAM)
						{
							(*isActive).exchange(false);
							break;
						}
					}
					catch (FatalException& e)
					{
						std::cerr << "FatalException during the " << filter->getName() << " execution. Msg : " << e.what() <<
							". Please contact us" << std::endl;

						destroyPythonThread(userThreadState);
						texptr = std::make_shared<std::exception_ptr>(std::current_exception());
						exit(-1);
					}
					catch (HipeException& e)
					{
						std::cerr << "HipeException during the " << filter->getName() << " execution. Msg : " << e.what() <<
							". Please contact us" << std::endl;
						cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);
						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the Streaming videocapture");


						destroyPythonThread(userThreadState);
						texptr = std::make_shared<std::exception_ptr>(std::current_exception());
						return;
					}

					catch (std::exception& e)
					{
						std::cerr << "Unkown error during the " << filter->getName() << " execution. Msg : " << e.what() <<
							". Please contact us" << std::endl;
						cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);

						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the Streaming videocapture");

						destroyPythonThread(userThreadState);
						texptr = std::make_shared<std::exception_ptr>(std::current_exception());
						return;
					}
					catch (...)
					{
						std::cerr << "Unkown error during the " << filter->getName() <<
							". Please contact us" << std::endl;
						cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);

						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the Streaming videocapture");
						destroyPythonThread(userThreadState);
						texptr = std::make_shared<std::exception_ptr>(std::current_exception());
						return;
					}
				}
				if (hipe_status == END_OF_STREAM)
				{
					(*isActive).exchange(false);
					break;
				}
			}

			//Special case for final result and other filter
			for (auto& filter : matrixLayer[matrixLayer.size() - 1])
			{
				if (filter == nullptr) continue;
				if (hipe_status == OK)
				{
					if (data::DataTypeMapper::isImage(sourceType) &&
						filter->getConstructorName().find("OutputRawDataFilter") != std::string::npos)
					{
						data::ConnexDataBase& outRes = filter->getConnector();
						data::OutputData outData;
						outData = static_cast<data::DataPort &>(outRes.getPort()).pop();
						outputData = static_cast<data::Data>(outData);
					}
					try
					{
						filter->process();
					}
					catch (...)
					{
						std::cerr << "Unkown error during the " << filter->getName() <<
							". Please contact us" << std::endl;
						cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);

						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the Streaming videocapture");
						destroyPythonThread(userThreadState);
						popUnusedData(filter);
						texptr = std::make_shared<std::exception_ptr>(std::current_exception());
						return;
					}
				}
				popUnusedData(filter);
			}

			if (data::DataTypeMapper::isImage(sourceType))
				break;
		} // while (isActive == true or not END_OF_STREAM)

		cleanDataChild(cpyFilterRoot);
		disposeChild(cpyFilterRoot);

		if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
			throw HipeException("Cannot free properly the videocapture");

		destroyPythonThread(userThreadState);
		//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	);
	if (data::DataTypeMapper::isVideo(sourceType))
	{
		runningTasks[0].task.reset(task);
		runningTasks[0].isActive.reset(isActive);
		runningTasks[0].filter.reset(cpyFilterRoot, [](filter::Model*)
	                             {
	                             });
		//taskInfo.texptr = texptr;

		/*runningTasks.push_back(taskInfo);*/

		//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	else
	{
		if (task->joinable())
			task->join();
		delete isActive;
		if (texptr)
		{
			std::rethrow_exception(*texptr);
		}
	}
}

void orchestrator::image::DefaultScheduler::updateFilterParameters(filter::Model* root, std::shared_ptr<filter::Model> model)
{
	updateParameters(root, model.get());
}

void orchestrator::image::DefaultScheduler::process(filter::Model* root, data::Data& inputData, data::Data& outputData, bool debug)
{
	if (!runningTasks.empty())
	{
		std::vector<TaskInfo> updateTasks;
		std::stringstream build;
		for (auto& taskInfo : runningTasks)
		{
			if (*(taskInfo.isActive) && (taskInfo.task->joinable()))
			{
				//It's an update of actual execution 
				if (taskInfo.filter->getName() == root->getName())
				{
					updateFilterParameters(root, taskInfo.filter);
					return;
				}
				updateTasks.push_back(taskInfo);
			}
			if (taskInfo.texptr)
			{
				try
				{
					std::rethrow_exception(*taskInfo.texptr);
				}
				catch (const std::exception& ex)
				{
					build << "Exception in previous task. Old Exception was : " << std::endl << ex.what();
					build << std::endl << std::endl;
				}
			}
		}
		runningTasks.clear();
		runningTasks = updateTasks;

		if (build.str() != "")
		{
			throw HipeException(build.str());
		}

		if (!runningTasks.empty())
			throw HipeException("Some previous task is stil running. Please kill it before run a new one.");
	}

	if (data::DataTypeMapper::isNoneData(inputData.getType()))
	{
		processDataSource(root, outputData, debug);
	}
	else
	{
		throw HipeException("Unknown type of data");
	}
}

void orchestrator::image::DefaultScheduler::killall()
{
	std::stringstream build;

	for (TaskInfo& taskInfo : runningTasks)
	{
		if (!taskInfo.isActive) continue;

		std::atomic<bool>* isActive = taskInfo.isActive.get();
		*(isActive) = false;
		if (taskInfo.task->joinable())
		{
			taskInfo.task->join();
			/*cleanDataChild(cpyFilterRoot);
			disposeChild(cpyFilterRoot);

			if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
				throw HipeException("Cannot free properly the videocapture");*/
		}
		if (taskInfo.texptr)
		{
			try
			{
				std::rethrow_exception(*taskInfo.texptr);
			}
			catch (const std::exception& ex)
			{
				build << "Some previous task generated an expection during execution. Old Exception was : " << std::endl << ex.what();
				build << std::endl << std::endl;
			}
		}
	}

	runningTasks.clear();

	if (build.str() != "")
	{
		throw HipeException(build.str());
	}
}

void orchestrator::image::DefaultScheduler::cleanDataChild(filter::Model* filter)
{
	filter->cleanUp();
	popUnusedData(filter);
	for (auto& childFilter : filter->getChildrens())
	{
		childFilter.second->cleanUp();

		cleanDataChild(childFilter.second);

		data::ConnexDataBase& outResChild = childFilter.second->getConnector();
		data::DataPort& data_portChild = static_cast<data::DataPort &>(outResChild.getPort());

		while (! data_portChild.empty())
		{
			data_portChild.pop();
		}
	}
}
