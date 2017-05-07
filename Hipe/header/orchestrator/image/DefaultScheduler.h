#pragma once
#include <filter/data/FileVideoInput.h>
#include <filter/data/ListIOData.h>
#include <filter/data/StreamVideoInput.h>
#include <boost/thread/thread.hpp>
#include "filter/data/ListIOData.h"

namespace orchestrator
{
	namespace image
	{
		class DefaultScheduler : public orchestrator::Conductor
		{
		public:
			typedef std::vector<std::vector<filter::IFilter *>> MatrixLayerNode;

			DefaultScheduler()
			{
			}

			static int getMaxLevelNode(filter::IFilter* filter, int level = 0)
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

			static int setMatrixLayer(filter::IFilter* filter, MatrixLayerNode& matrixLayerNode)
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

			static void cleanDataChild(filter::IFilter* filter)
			{
				filter->cleanUp();

				for (auto& childFilter : filter->getChildrens())
				{
					childFilter.second->cleanUp();
					cleanDataChild(childFilter.second);
				}
			}

			static void disposeChild(filter::IFilter* filter)
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
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				filter::IFilter* cpyFilterRoot = copyAlgorithms(filterRoot);
				
				
				std::shared_ptr<filter::data::StreamVideoInput> cpyVideo = std::make_shared<filter::data::StreamVideoInput>(video);
				boost::thread task([cpyFilterRoot, cpyVideo]()
				{
					int maxLevel = getMaxLevelNode(cpyFilterRoot->getRootFilter());
					std::shared_ptr<filter::data::Data> outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO : insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);
					cv::Mat frame = cpyVideo->newFrame();
					//TODO manage a buffering every things is here to do it
					//For now we just pick up one image
					while (!frame.empty())
					{
						cleanDataChild(cpyFilterRoot);
						//Special case for rootNode dispatching to any children.
						for (filter::IFilter * filter : matrixLayer[0])
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

				//task.join();
				
			}

			void processVideo(filter::Model* root, filter::data::Data& inputData, filter::data::Data & outputData, bool debug)
			{
				filter::data::FileVideoInput & video = static_cast<filter::data::FileVideoInput&>(inputData);
				cv::Mat frame;
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());
				MatrixLayerNode matrixLayer(maxLevel + 1);
				filter::IFilter* cpyFilterRoot = copyAlgorithms(filterRoot);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				std::shared_ptr<filter::data::FileVideoInput> cpyVideo = std::make_shared<filter::data::FileVideoInput>(video);

				boost::thread task([cpyFilterRoot, cpyVideo, maxLevel]()
				{
					//int maxLevel = getMaxLevelNode(cpyFilterRoot->getRootFilter());
					std::shared_ptr<filter::data::Data> thr_outputData;

					MatrixLayerNode matrixLayer(maxLevel + 1);

					//TODO insert debug layers into the matrix
					setMatrixLayer(cpyFilterRoot, matrixLayer);

					cv::Mat res = cpyVideo->newFrame();

					//TODO manage a buffering every things is here to do it
					//For now we just pick up one image
					while (!res.empty())
					{
						for (filter::IFilter * filter : matrixLayer[0])
						{
							
							*(filter) << res;
							
						}
						cleanDataChild(cpyFilterRoot);
		
						//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
						for (int layer = 1; layer < matrixLayer.size() - 1; layer++)
						{
							for (auto& filter : matrixLayer[layer])
							{
								filter->process();
							}
						}

						//Special case for final result
						matrixLayer[matrixLayer.size() - 1][0]->process();
						res = cpyVideo->newFrame();
					}
					cleanDataChild(cpyFilterRoot);
					disposeChild(cpyFilterRoot);
					if (freeAlgorithms(cpyFilterRoot) != HipeStatus::OK)
						throw HipeException("Cannot free properly the Streaming videocapture");
				});
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
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());

				cleanDataChild(filterRoot);

				MatrixLayerNode matrixLayer(maxLevel + 1);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				for (filter::IFilter * filter : matrixLayer[0])
				{
					
					*(filter) << inputData;
					
				}

				//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
				std::shared_ptr<filter::data::Data> inter_output;
				for (int layer = 1; layer < matrixLayer.size() - 1; layer++)
				{
					for (auto& filter : matrixLayer[layer])
					{
						
						filter->process();
						
						//pushOutputToChild(filter, *(inter_output.get()));
					}
				}
				
				//Special case for final result
				//Bug : What's happenning when the last layer has OutputFiltert and another Node (i.e : ShowImage)
				matrixLayer[matrixLayer.size() - 1][0]->process();
				for (auto& filter : matrixLayer[matrixLayer.size() - 1])
				{
					if (filter == nullptr) continue;

					if (filter->getConstructorName().find("ResultFilter") != std::string::npos)
					{
						filter::data::ConnexDataBase & outRes = filter->getConnector();
						outputData = (static_cast<filter::data::DataPort<filter::data::Data> &>(outRes.getPort())).pop();
						break;
					}
				}
				
				cleanDataChild(filterRoot);
				disposeChild(filterRoot);
			}


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
					processVideo(root, inputData, outputData, debug);
				}
				else if (filter::data::DataTypeMapper::isStreaming(inputData.getType()))
				{
					processStreaming(root, inputData, outputData, debug);
				}
				else
				{
					throw HipeException("Unknown type of data");
				}
				
			}
		};
	}
}
