#pragma once
#include <filter/data/FileVideoInput.h>
#include <filter/data/ListIOData.h>
#include <filter/data/StreamVideoInput.h>
#include <boost/thread/thread.hpp>
#include "filter/data/ListIOData.h"
#include <filter/data/PatternData.h>
#include <orchestrator/TaskInfo.h>


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

			/*static void pushOutputToChild(filter::IFilter* filter, filter::data::Data& io_data)
			{
				if (io_data.empty())
					return;

				for (auto& childFilter : filter->getChildrens())
				{
					if (childFilter.second->get_protect() == DataAccess::COPY)
					{
						filter::data::Data copy(io_data, true);
						childFilter.second->setInputData(copy);
					}
					else
					{
						childFilter.second->setInputData(io_data);
					}
				}
			}*/

			void processStreaming(filter::Model* root, filter::data::Data& inputData, filter::data::Data & outputData, bool debug)
			{
				const filter::data::StreamVideoInput & video = static_cast<const filter::data::StreamVideoInput&>(inputData);
				cv::Mat frame;
				filter::Model* filterRoot = reinterpret_cast<filter::Model *>(root);
				filter::Model* cpyFilterRoot = copyAlgorithms(filterRoot);
				std::atomic<bool> * isActive = new std::atomic<bool>(true);
				
				std::shared_ptr<filter::data::StreamVideoInput> cpyVideo = std::make_shared<filter::data::StreamVideoInput>(video);
				boost::thread * task = new boost::thread([cpyFilterRoot, cpyVideo, isActive]()
				{
					int maxLevel = getMaxLevelNode(cpyFilterRoot->getRootFilter());
					std::shared_ptr<filter::data::Data> outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO : insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);
					filter::data::Data frame = cpyVideo->newFrame();
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
			void processVideo(filter::Model* root, VideoClass& inputData, filter::data::Data & outputData, bool debug)
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
					std::shared_ptr<filter::data::Data> thr_outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);

					filter::data::Data res;
					
					res = cpyVideo->newFrame();

					//TODO manage a buffering every things is here to do it
					//For now we just pick up one image
					while (!res.empty() && (*isActive) == true)
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
									std::cerr << "Unkown error during the "<< filter->getName() << " execution. Msg : " << e.what() << ". Please contact us"  << std::endl;
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

							filter::data::ConnexDataBase & outRes = filter->getConnector();
							filter::data::DataPort & port = (static_cast<filter::data::DataPort &>(outRes.getPort()));
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


			void processSequence(filter::Model* root, filter::data::Data& inputData, filter::data::Data &outputData, bool debug)
			{
				if (filter::data::DataTypeMapper::isImage(inputData.getType()))
				{
					processImages(root, inputData, outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isVideo(inputData.getType()))
				{
					throw HipeException("processSequence of Video isn't yet implemented");
				}
			}

			void processListData(filter::Model* root, filter::data::ListIOData & inputData, filter::data::Data& outputData, bool debug)
			{
				if (inputData.getType() != filter::data::IODataType::LISTIO)
				{
					throw HipeException("cannot accept Data type other than List");
				}
				auto vecData = inputData.getListData();
				for (auto it = vecData.begin(); it != vecData.end(); ++it)
				{
					if (filter::data::DataTypeMapper::isImage(it->getType()))
					{
						processImages(root, *it, outputData, debug);
					}
					else if (filter::data::DataTypeMapper::isVideo(it->getType()))
					{
						throw HipeException("processSequence of Video isn't yet implemented");
					}
				}
			}

			void processImages(filter::Model* root, filter::data::Data & inputData, filter::data::Data &outputData, bool debug)
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
				std::shared_ptr<filter::data::Data> inter_output;
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
					
					if (filter->getConstructorName().find("ResultFilter") != std::string::npos)
					{
						filter::data::ConnexDataBase & outRes = filter->getConnector();
						outputData = (static_cast<filter::data::DataPort &>(outRes.getPort())).pop();
						
					}
					filter->process();
				}
				
				cleanDataChild(filterRoot);
				disposeChild(filterRoot);
			}


			void processPattern(filter::Model* root, filter::data::Data data, filter::data::Data& output_data, bool debug)
			{}

			void process(filter::Model* root, filter::data::Data& inputData, filter::data::Data &outputData, bool debug = false)
			{	
				
			    if (filter::data::DataTypeMapper::isSequence(inputData.getType()))
				{
					processSequence(root, inputData, outputData, debug);
				}
				if (filter::data::DataTypeMapper::isListIo(inputData.getType()))
				{
					filter::data::ListIOData &list_io_data = static_cast<filter::data::ListIOData&>(inputData);
					processListData(root, list_io_data, outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isImage(inputData.getType()))
				{
					processImages(root, inputData, outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isVideo(inputData.getType()))
				{
					using videoType = filter::data::FileVideoInput;

					if (inputData.getType() == filter::data::STRMVID)
						using videoType = filter::data::StreamVideoInput;

					processVideo(root, static_cast<videoType&>(inputData), outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isStreaming(inputData.getType()))
				{
					processStreaming(root, inputData, outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isPattern(inputData.getType()))
				{
					using videoType = filter::data::PatternData;
					processVideo(root, static_cast<videoType&>(inputData), outputData, debug);
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
