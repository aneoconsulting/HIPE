#pragma once
#include <string>
#include <map>
#include <filter/IFilter.h>
#include "json/JsonFilterNode/JsonFilterNode.h"
#include <core/HipeException.h>

namespace http
{
	namespace json
	{

		class JsonFilterTree
		{
			std::map<std::string, filter::IFilter *> _filterMap;

			//Check if the fully nodes are loaded;
			//If not no dependencies node computation can be accomplish
			bool isFreezed;
		public:
			JsonFilterTree() : isFreezed(false)
			{

			}
			JsonFilterTree(const JsonFilterTree & jTree) : isFreezed(false)
			{
				for (auto & node : jTree._filterMap)
				{
					_filterMap[node.first] = node.second;
				}
			}


			void freeze()
			{
				isFreezed = true; 
				computeLinkDependencies();
			}
			
			///
			/// Add Filter node and insert name of the dependent's parent inside the filter
			/// 
			void add(JsonFilterNode& filterNode)
			{
				filter::IFilter * filter = filterNode.getFilter();
				std::string name = filter->getName();

				if (_filterMap.find(name) != _filterMap.end())
				{
					std::string errorMessage = std::string("Filter named ");
					errorMessage += name;
					errorMessage += " doesn't exist";

					throw HipeException(errorMessage.c_str());
				}
				_filterMap[name] = filter;

				//Add dependencies name

				for (auto& parentName : filterNode.getDependenciesFilter())
				{
					filter->addDependenciesName(parentName);
				}
			}

			//Link every filters between them
			void computeLinkDependencies()
			{
				if (isFreezed == false) throw HipeException("Cannot compute dependencies of tree if the tree is not freezed");

				for (auto &it : _filterMap)
				{
					filter::IFilter * filterNode = it.second;
					const std::string name = it.first;

					for (auto & parent : filterNode->getParents())
					{
						//_filterMap[parent.first]->addChildDependencies(filterNode);
						filterNode->addDependencies(_filterMap[parent.first]);
					}
				}
				computeLevelNode();
			}

			//Compute the layer inder of the node when it will be executed
			void computeLevelNode()
			{
				if (isFreezed == false) throw HipeException("Cannot compute layer level of tree if the tree is not freezed");

				for (auto &it : _filterMap)
				{
					auto & filter = it.second;
					for (auto & child : filter->getChildrens())
					{
						int current_level = filter->getLevel();
						if (current_level >= child.second->getLevel())
						{
							child.second->setLevel(current_level + 1);
							computeLevelNode(); //Level do it for now recursively !!! Ugly 
							return;
						}
					}
				}
			}

		};
	}
}
