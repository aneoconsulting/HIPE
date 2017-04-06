#pragma once
#include <filter/data/FileVideoInput.h>

namespace orchestrator
{
	namespace image
	{
		class DefaultScheduler : public Conductor
		{
		public:
			typedef std::vector<std::vector<filter::IFilter *>> MatrixLayerNode;

			DefaultScheduler()
			{
			}

			int getMaxLevelNode(filter::IFilter* filter, int level = 0)
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

			int setMatrixLayer(filter::IFilter* filter, MatrixLayerNode& matrixLayerNode)
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

			void cleanDataChild(filter::IFilter* filter)
			{
				filter->cleanUp();

				for (auto& childFilter : filter->getChildrens())
				{
					childFilter.second->cleanUp();
					cleanDataChild(childFilter.second);
				}
			}

			void disposeChild(filter::IFilter* filter)
			{
				filter->dispose();

				for (auto& childFilter : filter->getChildrens())
				{
					childFilter.second->dispose();
					disposeChild(childFilter.second);
				}
			}

			void pushOutputToChild(filter::IFilter* filter, filter::data::IOData& io_data)
			{
				if (io_data.empty())
					return;

				for (auto& childFilter : filter->getChildrens())
				{
					if (childFilter.second->get_protect() == DataAccess::COPY)
					{
						filter::data::IOData copy(io_data, true);
						childFilter.second->setInputData(copy);
					}
					else
					{
						childFilter.second->setInputData(io_data);
					}
				}
			}

			void processStreaming(filter::Model* root, filter::data::IOData& inputData, filter::data::OutputData &outputData, bool debug)
			{
				throw HipeException("processStreaming of Data isn't yet implemented");
			}

			void processVideo(filter::Model* root, filter::data::IOData& inputData, std::shared_ptr<filter::data::IOData> & outputData, bool debug)
			{
				filter::data::FileVideoInput & video = filter::data::IOData::downCast<filter::data::FileVideoInput>(inputData);
				cv::Mat frame;
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				

				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());


				MatrixLayerNode matrixLayer(maxLevel + 1);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);


				//TODO manage a buffering every things is here to do it
				//For now we just pick up one image
				while(video.newFrame())
				{
					cleanDataChild(filterRoot);
					//Special case for rootNode dispatching to any children.
					for (auto& filter : matrixLayer[0])
					{
						pushOutputToChild(filter, inputData);
					}

					//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
					std::shared_ptr<filter::data::IOData> inter_output;
					for (int layer = 1; layer < matrixLayer.size() - 1; layer++)
					{
						for (auto& filter : matrixLayer[layer])
						{
							
							filter->process(inter_output);
							pushOutputToChild(filter, *inter_output.get());
						}
					}

					//Special case for final result
					matrixLayer[matrixLayer.size() - 1][0]->process(outputData);
				}
				cleanDataChild(filterRoot);
				disposeChild(filterRoot);
			}


			void processSequence(filter::Model* root, filter::data::IOData& inputData, std::shared_ptr<filter::data::IOData> &outputData, bool debug)
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

			void processImages(filter::Model* root, filter::data::IOData & inputData, std::shared_ptr<filter::data::IOData> &outputData, bool debug)
			{
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());

				cleanDataChild(filterRoot);

				MatrixLayerNode matrixLayer(maxLevel + 1);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				//Special case for rootNode dispatching to any children.
				for (auto& filter : matrixLayer[0])
				{
					pushOutputToChild(filter, inputData);
				}

				//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP
				std::shared_ptr<filter::data::IOData> inter_output;
				for (int layer = 1; layer < matrixLayer.size() - 1; layer++)
				{
					for (auto& filter : matrixLayer[layer])
					{
						
						filter->process(inter_output);
						if (inter_output.get() == nullptr)
							continue;
						pushOutputToChild(filter, *(inter_output.get()));
					}
				}
				
				//Special case for final result
				//Bug : What's happenning when the last layer has OutputFiltert and another Node (i.e : ShowImage)
				matrixLayer[matrixLayer.size() - 1][0]->process(outputData);
				cleanDataChild(filterRoot);
				disposeChild(filterRoot);
			}


			void process(filter::Model* root, std::shared_ptr<filter::data::IOData>& inputDataPtr, std::shared_ptr<filter::data::IOData> &outputData, bool debug = false)
			{
				filter::data::IOData & inputData = *inputDataPtr.get();

				if (filter::data::DataTypeMapper::isSequence(inputData.getType()))
				{
					processSequence(root, inputData, outputData, debug);
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
					throw HipeException("Streaming isn't yet implemented");
				}
				else
				{
					throw HipeException("Unknown type of data");
				}
				
			}
		};
	}
}
