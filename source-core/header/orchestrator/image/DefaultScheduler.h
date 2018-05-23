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

#pragma warning(push, 0)
#include <boost/thread/thread.hpp>
#include <Python.h>
#pragma warning(pop)


namespace orchestrator
{
	namespace image
	{
		class PyAllowThreads
		{
		public:
			PyAllowThreads() :
				_state(PyEval_SaveThread())
			{
			}

			~PyAllowThreads()
			{
				PyEval_RestoreThread(_state);
			}

		private:
			PyThreadState* _state;
		};

		

		class DefaultScheduler : public orchestrator::Conductor
		{
		public:
			typedef std::vector<std::vector<filter::Model *>> MatrixLayerNode;
			std::vector<TaskInfo> runningTasks;
			PyThreadState* pyMainThreaState;
			PyThreadState* pyThreadState;
			
			PyInterpreterState* interp;
			

			DefaultScheduler()
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

			static void cleanDataChild(filter::Model* filter)
			{
				filter->cleanUp();
				data::ConnexDataBase& outRes = filter->getConnector();
				data::DataPort& data_port = static_cast<data::DataPort &>(outRes.getPort());

				while (! data_port.empty())
				{
					data_port.pop();
				}

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

			static void disposeChild(filter::Model* filter)
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

			static void InitNewPythonThread(PyInterpreterState* interpreterPython, PyExternalUser * & pyExternalUser)
			{
				if (! pyExternalUser )
				{
					
					//interp = ts->interp;
					//PyThreadState_Swap(pyThreadState);
					pyExternalUser = new PyExternalUser(interpreterPython);
				}
			}

			static void setPythonUserThreadState(filter::Model* model, PyExternalUser* py_external_user)
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

			static void destroyPythonThread(PyExternalUser * & pyExternalUser)
			{
				if (pyExternalUser)
				{
					delete pyExternalUser;
					pyExternalUser = nullptr;
				}
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
				PyInterpreterState* l_interp = interp;

				boost::thread* task = new boost::thread([cpyFilterRoot, maxLevel, &outputData, sourceType, isActive, l_interp]()
				{
					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);
					//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
					//std::shared_ptr<data::Data> inter_output;
					HipeStatus hipe_status = OK;

					//Create new python ThreadState for filter which need it
					PyExternalUser *userThreadState = nullptr;
					InitNewPythonThread(l_interp, userThreadState);

					setPythonUserThreadState(cpyFilterRoot, userThreadState);

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
										break;
								}
								catch (HipeException& e)
								{
									std::cerr << "HipeException error during the " << filter->getName() << " execution. Msg : " << e.what() <<
										". Please contact us" << std::endl;
									cleanDataChild(cpyFilterRoot);
									disposeChild(cpyFilterRoot);
									if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
										throw HipeException("Cannot free properly the Streaming videocapture");
									

									destroyPythonThread(userThreadState);
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
									return;
								}
							}
							if (hipe_status == END_OF_STREAM)
								break;
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
								filter->process();
							}
						}

						if (data::DataTypeMapper::isImage(sourceType))
							break;
					}
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
					TaskInfo taskInfo;
					taskInfo.task.reset(task);
					taskInfo.isActive.reset(isActive);
					taskInfo.filter.reset(cpyFilterRoot, [](filter::Model*)
				                      {
				                      });
					runningTasks.push_back(taskInfo);
					//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
				}
				else
				{
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
						/*cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);

						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the videocapture");*/
					}
				}
				runningTasks.clear();
			}
		};
	}
}
