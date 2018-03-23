#pragma once
#include <coredata/OutputData.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <orchestrator/TaskInfo.h>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/datasource/DataSource.h>
#include <algorithm>
#include <thread>


namespace orchestrator
{
	namespace image
	{
		class DefaultScheduler : public orchestrator::Conductor
		{
		public:
			typedef std::vector<std::vector<filter::Model *>> MatrixLayerNode;
			std::vector<TaskInfo> runningTasks;

			DefaultScheduler()
			{
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

				for (auto& childFilter : filter->getChildrens())
				{
					childFilter.second->cleanUp();
					cleanDataChild(childFilter.second);
				}
			}

			static void disposeChild(filter::Model* filter)
			{
				filter->dispose();

				for (auto& childFilter : filter->getChildrens())
				{
					childFilter.second->dispose();
					disposeChild(childFilter.second);
				}
			}
		
			/*void processStreaming(filter::Model* root, data::Data& inputData, data::Data & outputData, bool debug)
			{
				const data::StreamVideoInput & video = static_cast<const data::StreamVideoInput&>(inputData);
				cv::Mat frame;
				filter::Model* filterRoot = reinterpret_cast<filter::Model *>(root);
				filter::Model* cpyFilterRoot = copyAlgorithms(filterRoot);
				std::atomic<bool> * isActive = new std::atomic<bool>(true);

				std::shared_ptr<data::StreamVideoInput> cpyVideo = std::make_shared<data::StreamVideoInput>(video);
				boost::thread * task = new boost::thread([cpyFilterRoot, cpyVideo, isActive]()
				{
					int maxLevel = getMaxLevelNode(cpyFilterRoot->getRootFilter());
					std::shared_ptr<data::Data> outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO : insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);
					data::Data frame = cpyVideo->newFrame();
					//TODO manage a buffering every things is here to do it
					//For now we just pick up one image
					while (!frame.empty() && (*isActive) == true)
					{
						cleanDataChild(cpyFilterRoot);
						//Special case for rootNode dispatching to any children.
						for (filter::Model * filter : matrixLayer[0])
						{
							*(filter) << frame;
						}

						//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
						for (int layer = 1; layer < matrixLayer.size(); layer++)
						{
							for (auto& filter : matrixLayer[layer])
							{
								filter->process();
							}
						}

						frame = cpyVideo->newFrame();
					}

					cleanDataChild(cpyFilterRoot);

					disposeChild(cpyFilterRoot);

					if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
						throw HipeException("Cannot free properly the Streaming videocapture");
				});
				TaskInfo taskInfo;
				taskInfo.task.reset(task);
				taskInfo.isActive.reset(isActive);
				taskInfo.filter.reset(cpyFilterRoot, [](filter::Model *) {});
				runningTasks.push_back(taskInfo);
				//task.join();

			}

			template<class VideoClass>
			void processVideo(filter::Model* root, VideoClass& inputData, data::Data & outputData, bool debug)
			{
				VideoClass & video = static_cast<VideoClass&>(inputData);
				cv::Mat frame;
				filter::Model* filterRoot = root;
				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());
				MatrixLayerNode matrixLayer(maxLevel + 1);
				filter::Model* cpyFilterRoot = copyAlgorithms(filterRoot);
				std::atomic<bool> * isActive = new std::atomic<bool>(true);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				std::shared_ptr<VideoClass> cpyVideo = std::make_shared<VideoClass>(video);

				boost::thread *task = new boost::thread([cpyFilterRoot, cpyVideo, maxLevel, isActive]()
				{
					//int maxLevel = getMaxLevelNode(cpyFilterRoot->getRootFilter());
					std::shared_ptr<data::Data> thr_outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);

					data::Data res;


					try
					{
						res = cpyVideo->newFrame();
					}
					catch (HipeException& e)
					{
						std::cerr << "HipeException error getting the first video frame : execution. Msg : " << e.what() <<
							". Please contact us" << std::endl;
						cleanDataChild(cpyFilterRoot);
						disposeChild(cpyFilterRoot);
						if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
							throw HipeException("Cannot free properly the Streaming videocapture");
						return;
					}
					//TODO manage a buffering every things is here to do it
					//For now we just pick up one image
					while (!res.empty() && (*isActive))
					{
						for (filter::Model * filter : matrixLayer[0])
						{

							*(filter) << res;

						}
						cleanDataChild(cpyFilterRoot);

						//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
						for (unsigned int layer = 1; layer < matrixLayer.size() - 1; layer++)
						{
							for (auto& filter : matrixLayer[layer])
							{
								try
								{
									filter->process();
								}
								catch (HipeException& e) {
									std::cerr << "HipeException error during the " << filter->getName() << " execution. Msg : " << e.what() << ". Please contact us" << std::endl;
									cleanDataChild(cpyFilterRoot);
									disposeChild(cpyFilterRoot);
									if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
										throw HipeException("Cannot free properly the Streaming videocapture");
									return;
								}

								catch (std::exception& e) {
									std::cerr << "Unkown error during the " << filter->getName() << " execution. Msg : " << e.what() << ". Please contact us" << std::endl;
									cleanDataChild(cpyFilterRoot);
									disposeChild(cpyFilterRoot);
									if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
										throw HipeException("Cannot free properly the Streaming videocapture");
									return;
								}

							}
						}

						//Special case for final result and other filter
						for (auto& filter : matrixLayer[matrixLayer.size() - 1])
						{
							if (filter == nullptr) continue;


							filter->process(); // Nothing to keep we are in async and detach task. Just execute

							data::ConnexDataBase & outRes = filter->getConnector();
							data::DataPort & port = (static_cast<data::DataPort &>(outRes.getPort()));
							while (port.size() != 0)
							{
								port.pop();
							}
						}
						res.release();
						res = cpyVideo->newFrame();
					}

					cleanDataChild(cpyFilterRoot);
					disposeChild(cpyFilterRoot);
					if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
						throw HipeException("Cannot free properly the Streaming videocapture");
				});
				TaskInfo taskInfo;
				taskInfo.task.reset(task);
				taskInfo.isActive.reset(isActive);
				taskInfo.filter.reset(cpyFilterRoot, [](filter::Model *) {});
				runningTasks.push_back(taskInfo);
				//task->join();
			}


			void processSequence(filter::Model* root, data::Data& inputData, data::Data &outputData, bool debug)
			{
				if (data::DataTypeMapper::isVideo(inputData.getType()))
				{
					throw HipeException("processSequence of Video isn't yet implemented");
				}

				//TMI: HACK: Workaround FilePatternFilter
				if (inputData.getType() == data::IODataType::SEQIMGD)
				{
					data::DirectoryImgData & dirImgData = static_cast<data::DirectoryImgData &>(inputData);
					dirImgData.loadImagesData();
				}
				processImages(root, inputData, outputData, debug);
			}

			void processListData(filter::Model* root, data::ListIOData & inputData, data::Data& outputData, bool debug)
			{
				if (inputData.getType() != data::IODataType::LISTIO)
				{
					throw HipeException("cannot accept Data type other than List");
				}
				auto vecData = inputData.getListData();
				for (auto it = vecData.begin(); it != vecData.end(); ++it)
				{
					if (data::DataTypeMapper::isImage(it->getType()))
					{
						processImages(root, *it, outputData, debug);
					}
					else if (data::DataTypeMapper::isVideo(it->getType()))
					{
						throw HipeException("processSequence of Video isn't yet implemented");
					}
				}
			}

			void processImages(filter::Model* root, data::Data & inputData, data::Data &outputData, bool debug)
			{
				filter::Model* filterRoot = reinterpret_cast<filter::Model *>(root);
				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());

				cleanDataChild(filterRoot);

				MatrixLayerNode matrixLayer(maxLevel + 1);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				for (filter::Model * filter : matrixLayer[0])
				{
					*(filter) << inputData;
				}

				//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
				std::shared_ptr<data::Data> inter_output;
				for (unsigned int layer = 1; layer < matrixLayer.size() - 1; layer++)
				{
					for (auto& filter : matrixLayer[layer])
					{

						filter->process();

						//pushOutputToChild(filter, *(inter_output.get()));
					}
				}

				//Special case for final result and other filter
				for (auto& filter : matrixLayer[matrixLayer.size() - 1])
				{
					if (filter == nullptr) continue;

					if (filter->getConstructorName().find("OutputRawDataFilter") != std::string::npos)
					{
						data::ConnexDataBase & outRes = filter->getConnector();
						data::OutputData outData;
						outData = static_cast<data::DataPort &>(outRes.getPort()).pop();
						outputData = outData;

					}
					filter->process();
				}

				cleanDataChild(filterRoot);
				disposeChild(filterRoot);
			}


			void processPattern(filter::Model* root, data::Data data, data::Data& output_data, bool debug)
			{}*/

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
				filter::Model* cpyFilterRoot = copyAlgorithms(filterRoot);

				for (auto dataSource : matrixLayer[1])
				{
					filter::datasource::DataSource* data_source = dynamic_cast<filter::datasource::DataSource*>(dataSource);
					if (data_source != nullptr)
					{
						if (data_source->getSourceType() == data::IODataType::VIDF)
						{
							sourceType = data::IODataType::VIDF;;
							break;
						}
					}
				}

				
				std::atomic<bool>* isActive = new std::atomic<bool>(true);
				//std::thread thr1;
				


				boost::thread* task = new boost::thread([cpyFilterRoot, maxLevel, &outputData, sourceType, isActive]()
				{
					std::cout<<"start"<<std::endl;
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
					unsigned concurentThreadsSupported = (std::max)(NbminCore,std::thread::hardware_concurrency());
					unsigned nbThrPoool = FactCore * concurentThreadsSupported;
					//std::cout << nbThrPoool << "threads" <<std::endl;
					boost::mutex mutexHipeStatus;
					boost::mutex mutexcpyFilter;	
					int range =0;
					
					// Creation de pool de thread == 1-2 fois le nb de core disponible hors HyperThreading 
					std::vector<int> pool(nbThrPoool, -1);
					/*
					Version pour mettre en pause puis relancer les threads
					*/
					/*std::vector<std::vector<boost::thread*>> threadRange(matrixLayer.size() - 1)*/
							
					while ((hipe_status != END_OF_STREAM) && *isActive)
					{
						range +=1;
						//std::cout << "start range: "<<range << std:: endl;
						for (unsigned int layer = 1; layer < matrixLayer.size() - 1; layer++)
						{
							
							std::vector<boost::thread*> threadLayer(matrixLayer[layer].size());
							for (int i = 0; i < matrixLayer[layer].size() ; i++)
							{
								while(find(pool.begin(), pool.end(), -1) == pool.end())
								{
									std::cout<<"wait" <<std::endl;
								}
								std::vector<int>::iterator it;
								it = find(pool.begin(), pool.end(), -1);
								auto& cpyFilter = matrixLayer[layer][i];
								//std::cout << "start: " << it-pool.begin() << std::endl;
								*it = i;
								threadLayer[i] = new boost::thread([&cpyFilter, cpyFilterRoot, &outputData, &hipe_status, &pool, it, &mutexHipeStatus, &mutexcpyFilter]()
								{
									try
									{
										mutexHipeStatus.lock();
										//std::cout << "start pool" <<*it   <<std::endl;
										hipe_status = cpyFilter->process();
										if (hipe_status == END_OF_STREAM)
											std::cout << "end of stream" <<std::endl;
										mutexHipeStatus.unlock();
										*it = -1;
										//break;
									}
									catch (HipeException& e)
									{
										std::cerr << "HipeException error during the " << cpyFilter->getName() << " execution. Msg : " << e.what() <<
										". Please contact us" << std::endl;
										mutexcpyFilter.lock();
										cleanDataChild(cpyFilterRoot);
										disposeChild(cpyFilterRoot);
										if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
											throw HipeException("Cannot free properly the Streaming videocapture");
										mutexcpyFilter.unlock();
										return;
									}

									catch (std::exception& e)
									{
										std::cerr << "Unkown error during the " << cpyFilter->getName() << " execution. Msg : " << e.what() <<
										". Please contact us" << std::endl;
										mutexcpyFilter.lock();
										cleanDataChild(cpyFilterRoot);
										disposeChild(cpyFilterRoot);
										if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
											throw HipeException("Cannot free properly the Streaming videocapture");
										mutexcpyFilter.unlock();
										return;
									}
									//std::cout << "end pool"  <<std::endl;
									
								});
																
								
								
								

								//std::cout << "changement de pool"<<std::endl;
							}
							
							for (int j = 0; j < matrixLayer[layer].size() ; j++)
							{
								//std::cout <<"start join: "<<j <<std::endl;
								if (threadLayer[j] ->joinable())
								{
									threadLayer[j]->join();
									
								}
							}
							threadLayer.clear();	
							
							if (hipe_status == END_OF_STREAM){
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
					//std::cout << "End while: "<< range <<std::endl;
					cleanDataChild(cpyFilterRoot);
					//std::cout << "clean" <<std::endl;
					disposeChild(cpyFilterRoot);
					//std::cout << "dispose"<<std::endl;

					if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK){
					//std::cout << "if end thread"<<std::endl;
						throw HipeException("Cannot free properly the videocapture");
					}
					//std::cout << "end thread"<<std::endl;
				});
				if (data::DataTypeMapper::isVideo(sourceType))
				{
					//std::cout << "isvideo "<<std::endl;
					TaskInfo taskInfo;
					taskInfo.task.reset(task);
					taskInfo.isActive.reset(isActive);
					taskInfo.filter.reset(cpyFilterRoot, [](filter::Model *) {});
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

			void process(filter::Model* root, data::Data& inputData, data::Data &outputData, bool debug = false)
			{

				/*if (data::DataTypeMapper::isSequence(inputData.getType()))
				{
					processSequence(root, inputData, outputData, debug);
				}
				else if (data::DataTypeMapper::isListIo(inputData.getType()))
				{
					data::ListIOData &list_io_data = static_cast<data::ListIOData&>(inputData);
					processListData(root, list_io_data, outputData, debug);
				}
				else if (data::DataTypeMapper::isImage(inputData.getType()))
				{
					processImages(root, inputData, outputData, debug);
				}
				else if (data::DataTypeMapper::isVideo(inputData.getType()))
				{
					using videoType = data::FileVideoInput;

					if (inputData.getType() == data::STRMVID)
						using videoType = data::StreamVideoInput;

					processVideo(root, static_cast<videoType&>(inputData), outputData, debug);
				}
				else if (data::DataTypeMapper::isStreaming(inputData.getType()))
				{
					processStreaming(root, inputData, outputData, debug);
				}
				else if (data::DataTypeMapper::isPattern(inputData.getType()))
				{
					using videoType = data::PatternData;
					using videoDir = data::DirPatternData;
					if (inputData.getType() == data::IODataType::DIRPATTERN)
						processVideo(root, static_cast<videoDir&>(inputData), outputData, debug);
					else
						processVideo(root, static_cast<videoType&>(inputData), outputData, debug);
				}
				else */if (data::DataTypeMapper::isNoneData(inputData.getType()))
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
				for (TaskInfo & taskInfo : runningTasks)
				{
					std::atomic<bool>* isActive = taskInfo.isActive.get();
					*(isActive) = false;
					if (taskInfo.task->joinable())
						taskInfo.task->join();


				}
				runningTasks.clear();
			}
			
			
		};
	}
}
