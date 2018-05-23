//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <core/python/pyThreadSupport.h>
#include <coredata/OutputData.h>
#include <orchestrator/TaskInfo.h>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/datasource/DataSource.h>
#include <algorithm>
#include <thread>
#pragma warning(push, 0)
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <Python.h>

#include <boost/asio/io_service.hpp>
#include <orchestrator/image/TasksPool.h>
#pragma warning(pop)


namespace orchestrator
{
	namespace image
	{
		class SchedulerSimonV2 : public orchestrator::Conductor
		{
		public:
			typedef std::vector<std::vector<filter::Model *>> MatrixLayerNode;
			std::vector<TaskInfo> runningTasks;
			PyThreadState* pyMainThreaState;
			PyThreadState* pyThreadState;

			PyInterpreterState* interp;

			SchedulerSimonV2()
			{
				pyMainThreaState = nullptr;
				interp = nullptr;
				pyThreadState = nullptr;
			}

			static int getMaxLevelNode(filter::Model* filter, int level = 0)
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

			static int setMatrixLayer(filter::Model* filter, MatrixLayerNode& matrixLayerNode)
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

			static void cleanDataChild(filter::Model* filter, bool recursively = true)
			{
				filter->cleanUp();
				data::ConnexDataBase& outRes = filter->getConnector();
				data::DataPort& data_port = static_cast<data::DataPort &>(outRes.getPort());

				while (! data_port.empty())
				{
					data_port.pop();
				}

				if (recursively)
				{
					for (auto& childFilter : filter->getChildrens())
					{
						childFilter.second->cleanUp();
						cleanDataChild(childFilter.second);
						data::ConnexDataBase& outResChild = childFilter.second->getConnector();
						data::DataPort& data_portChild = static_cast<data::DataPort &>(outResChild.getPort());

						while (!data_portChild.empty())
						{
							data_portChild.pop();
						}
					}
				}
			}

			static void disposeChild(filter::Model* filter, bool recursively = true)
			{
				filter->dispose();

				data::ConnexDataBase& outRes = filter->getConnector();
				data::DataPort& data_port = static_cast<data::DataPort &>(outRes.getPort());

				while (! data_port.empty())
				{
					data_port.pop();
				}
				if (recursively)
				{
					for (auto& childFilter : filter->getChildrens())
					{
						childFilter.second->dispose();
						disposeChild(childFilter.second);
						data::ConnexDataBase& outResChild = childFilter.second->getConnector();
						data::DataPort& data_portChild = static_cast<data::DataPort &>(outResChild.getPort());

						while (!data_portChild.empty())
						{
							data_portChild.pop();
						}
					}
				}
			}

			void InitPythonEngine(filter::Model* filter, int level = 0)
			{
				if (!Py_IsInitialized())
				{
					Py_Initialize();
					// Create GIL/enable threads
					PyEval_InitThreads();
					pyMainThreaState = PyGILState_GetThisThreadState();
					PyEval_ReleaseThread(pyMainThreaState);
				}
				//// Get the default thread state  

				pyMainThreaState = PyGILState_GetThisThreadState();


				if (level == 0 && pyThreadState == nullptr)
				{
					PyEval_AcquireThread(pyMainThreaState);
					pyThreadState = Py_NewInterpreter();

					PyThreadState_Swap(pyMainThreaState);
					PyEval_ReleaseThread(pyMainThreaState);
				}


				if (filter->isPython())
				{
					PyEval_AcquireThread(pyMainThreaState);
					interp = pyThreadState->interp;
					filter->onLoad(interp);
					PyEval_ReleaseThread(pyMainThreaState);
				}


				for (auto& childFilter : filter->getChildrens())
				{
					InitPythonEngine(childFilter.second, level + 1);
				}

				if (level == 0)
				{
					pyThreadState = nullptr;
				}
			}

			static std::pair<boost::thread::id, PyExternalUser*> InitNewPythonThread(PyInterpreterState* interpreterPython)
			{
				std::pair<boost::thread::id, PyExternalUser *> result;
				result.first = boost::this_thread::get_id();
				result.second = new PyExternalUser(interpreterPython);
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
				return result;
			}

			/*static void setPythonUserThreadState(filter::Model* model, PyExternalUser* py_external_user, bool recursively = true)
			{
				if (model->isPython())
				{
					model->onStart(py_external_user);
				}

				if (recursively)
				{
					for (auto& childFilter : model->getChildrens())
					{
						setPythonUserThreadState(childFilter.second, py_external_user);
					}
				}
			}*/

			static int setPythonUserThreadState(filter::Model* model, std::map<boost::thread::id, PyExternalUser *> & pyThreadStates)
			{
				if (model->isPython())
				{
					PyExternalUser *userThreadState = pyThreadStates[boost::this_thread::get_id()];
					if (userThreadState == nullptr)
					{
						HipeException("Cannot find Python threadState for the current threadId");
					}

					model->onStart(userThreadState);
				}
				

				return 0;
			}

			static void destroyPythonThread(std::map<boost::thread::id, PyExternalUser*> & pyExternalUsers)
			{

				for (auto &pair : pyExternalUsers)
				{
					PyExternalUser* pyExternalUser  = pair.second;

					if (pyExternalUser)
					{
						delete pyExternalUser;
						pair.second = nullptr;
					}

				}
			}

		

			static std::map<boost::thread::id, PyExternalUser *> startTaskPool(TasksPool &tasksPool, PyInterpreterState* interpreterPython)
			{
				typedef boost::packaged_task<std::pair<boost::thread::id, PyExternalUser*> >   internal_task_t;
				typedef boost::shared_ptr<internal_task_t>                                     p_ininternal_task_t;
				typedef std::pair<boost::thread::id, PyExternalUser*>						   result_t;

				std::vector<boost::shared_future<result_t> > pending_data;

				//InitNewThreadState for python in c++ Thread pool
				for (int i = 0; i < boost::thread::hardware_concurrency(); ++i)
				{
					p_ininternal_task_t task = boost::make_shared<internal_task_t>(boost::bind(&InitNewPythonThread, interpreterPython));
					boost::shared_future<result_t> fut(task->get_future());
					pending_data.push_back(fut);
					tasksPool.getIoService().post(boost::bind(&internal_task_t::operator(), task));
					
				}

				boost::wait_for_all(pending_data.begin(), pending_data.end());
				
				std::map<boost::thread::id, PyExternalUser *> threadStates;

				//Get all threadStates coming from Thread itself
				for(boost::shared_future<result_t> res : pending_data)
				{
					result_t pair = res.get();
					threadStates[pair.first] = pair.second;
				}

				return threadStates;
			}

			/**
			 * \brief The task coming from the pool to delegate the filter processing
			 * \param cpyFilter the copy of current filter to execute
			 * \param cpyFilterRoot the copy of the entire graph
			 * \param outputData the result if needed
			 * \param pyThreadStates the pyhton state by all cuurent threadId
			 * \return 0
			 */
			static int runThreadProcess(filter::Model *&cpyFilter, 
				filter::Model *cpyFilterRoot, 
				std::map<boost::thread::id, PyExternalUser *> pyThreadStates)
			{
				setPythonUserThreadState(cpyFilter, pyThreadStates);
				
				try
				{
					HipeStatus hipe_status = cpyFilter->process();
					hipe_status = hipe_status;
					if (hipe_status == END_OF_STREAM)
						std::cout << "end of stream" << std::endl;
				}
				catch (HipeException& e)
				{
					std::cerr << "HipeException error during the " << cpyFilter->getName() << " execution. Msg : " << e.what() <<
						". Please contact us" << std::endl;
					cleanDataChild(cpyFilter, false);
					disposeChild(cpyFilter, false);
					//TO DO FORWARD EXCEPTION !!!!
					return -1;
				}

				catch (std::exception& e)
				{
					std::cerr << "Unkown error during the " << cpyFilter->getName() << " execution. Msg : " << e.what() <<
						". Please contact us" << std::endl;
					
					cleanDataChild(cpyFilter, false);
					disposeChild(cpyFilter, false);
					//TO DO FORWARD EXCEPTION !!!!
					
					return -1;
				}
				//std::cout << "end pool"  <<std::endl;
				return 0;
			}

			void processDataSource(filter::Model* root, data::Data& outputData, bool debug)
			{
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
				InitPythonEngine(cpyFilterRoot);
				std::atomic<bool>* isActive = new std::atomic<bool>(true);
				//std::thread thr1;

				PyInterpreterState* l_interp = interp;

				boost::thread* task = new boost::thread([cpyFilterRoot, maxLevel, &outputData, sourceType, isActive, l_interp]()
				{
					std::cout << "start" << std::endl;
					// ALlocation std::vector<std::vector>>(MAXLEVEL + 1)
					//Profondeur de graphe base sur la le plus grand nombre de dependances
					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);
					// | X     |
					// | X X X |
					// | X X   |
					// | X X X |
					// | X     |

					//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
					//std::shared_ptr<data::Data> inter_output;
					HipeStatus hipe_status = OK;


					//Detection du nombre core
					//With C++11; may return 0 when not able detect to
					unsigned NbminCore = 1;
					unsigned FactCore = 1;
					unsigned concurentThreadsSupported = (std::max)(NbminCore, std::thread::hardware_concurrency());
					unsigned nbThrPoool = FactCore * concurentThreadsSupported;
					//std::cout << nbThrPoool << "threads" <<std::endl;
					boost::mutex mutexHipeStatus;
					boost::mutex mutexcpyFilter;
					
					try
					{
						// Creation de pool de thread == 1-2 fois le nb de core disponible hors HyperThreading 
						TasksPool tasksPool;
						std::auto_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(tasksPool.getIoService()));

						std::map<boost::thread::id, PyExternalUser*> pyThreadStates = startTaskPool(tasksPool, l_interp);

						while ((hipe_status != END_OF_STREAM) && *isActive)
						{

							//std::cout << "start range: "<<range << std:: endl;
							for (unsigned int layer = 1; layer < matrixLayer.size() - 1; layer++)
							{
								std::vector<boost::shared_future<int>> pending_data;

								for (int i = 0; i < matrixLayer[layer].size(); i++)
								{



									auto& cpyFilter = matrixLayer[layer][i];
									//std::cout << "start: " << it-pool.begin() << std::endl;

									//Run job
									ptask_t task = boost::make_shared<task_t>(boost::bind(&runThreadProcess, cpyFilter, cpyFilterRoot, pyThreadStates));

									tasksPool.push_job(task, pending_data);
									//std::cout << "changement de pool"<<std::endl;
								}

								boost::wait_for_all(pending_data.begin(), pending_data.end());

								if (hipe_status == END_OF_STREAM)
								{
									//std::cout << "end of stream" <<std::endl;
									break;
								}
							}
							//std::cout << "start size -1 "<< range <<std::endl;
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
									filter->process();
								}
							}
							//std::cout << "fin size -1 "<<std::endl;
							if (data::DataTypeMapper::isImage(sourceType))
								break;
							//std::cout << "fin range: "<< range <<std::endl;
						}


						work.reset();
						if (!tasksPool.getIoService().stopped())
						{
							tasksPool.getIoService().stop();
							int retry = 100;
							while ((!tasksPool.getIoService().stopped()) && retry >= 0)
							{
								boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
							}
							if (!tasksPool.getIoService().stopped())
							{
								HipeException("Fail to stop MultiThread Scheduler");
							}
						}

						destroyPythonThread(pyThreadStates);

						//std::cout << "End while: "<< range <<std::endl;
						cleanDataChild(cpyFilterRoot);
						//std::cout << "clean" <<std::endl;
						disposeChild(cpyFilterRoot);
						//std::cout << "dispose"<<std::endl;

						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
						{
							//std::cout << "if end thread"<<std::endl;
							throw HipeException("Cannot free properly the videocapture");
						}


						
					}
					catch (std::exception &e)
					{
						throw HipeException(e.what());
					}
					//std::cout << "end thread"<<std::endl;
				});
				if (data::DataTypeMapper::isVideo(sourceType))
				{
					//std::cout << "isvideo "<<std::endl;
					TaskInfo taskInfo;
					taskInfo.task.reset(task);
					taskInfo.isActive.reset(isActive);
					taskInfo.filter.reset(cpyFilterRoot, [](filter::Model*)
				                      {
				                      });
					runningTasks.push_back(taskInfo);
				}
				else
				{
					//std::cout << "is not video "<<std::endl;
					if (task->joinable())
						task->join();

					delete isActive;
				}
			}

			void process(filter::Model* root, data::Data& inputData, data::Data& outputData, bool debug = false)
			{
				if (!runningTasks.empty())
				{
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

			virtual void killall()
			{
				for (TaskInfo& taskInfo : runningTasks)
				{
					std::atomic<bool>* isActive = taskInfo.isActive.get();
					*(isActive) = false;
					if (taskInfo.task->joinable())
					{
						taskInfo.task->join();
					}
				}
				runningTasks.clear();
			}
		};
	}
}
