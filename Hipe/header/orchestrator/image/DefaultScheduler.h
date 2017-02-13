#pragma once

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

			void pushOutputToChild(filter::IFilter* filter, filter::data::IOData& io_data)
			{
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

			

			void process(filter::Model* root, filter::data::IOData& inputData, filter::data::OutputData &outputData, bool debug = false)
			{
				filter::IFilter* filterRoot = reinterpret_cast<filter::IFilter *>(root);
				cleanDataChild(filterRoot);

				int maxLevel = getMaxLevelNode(filterRoot->getRootFilter());


				MatrixLayerNode matrixLayer(maxLevel + 1);

				//TODO insert debug layers into the matrix
				setMatrixLayer(filterRoot, matrixLayer);

				//Special case for rootNode dispatching to any children.
				for (auto& filter : matrixLayer[0])
				{
					pushOutputToChild(filter, inputData);
				}

				//TODO : Sort split layer when 2 nodes are trying to execute on GPU or OMP

				for (int layer = 1; layer < matrixLayer.size() - 1; layer++)
				{
					for (auto& filter : matrixLayer[layer])
					{
						filter::data::IOData inter_output;
						filter->process(inter_output);
						pushOutputToChild(filter, inter_output);
					}
				}

				//Special case for final result
				matrixLayer[matrixLayer.size() - 1][0]->process(outputData);


				
			}
		};
	}
}
