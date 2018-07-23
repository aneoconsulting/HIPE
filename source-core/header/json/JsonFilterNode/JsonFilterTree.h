//@HIPE_LICENSE@
#pragma once
#include <map>
#include <corefilter/Model.h>
#include <json/JsonFilterNode/JsonFilterNode.h>
#include <core/HipeException.h>
#include <json/json_export.h>

namespace json
{
	class JSON_EXPORT JsonFilterTree : public filter::Model
	{
		std::map<std::string, Model *> _filterMap;

		//Check if the fully nodes are loaded;
		//If not no dependencies node computation can be accomplish
		bool isFreezed;

	public:
		JsonFilterTree() : isFreezed(false)
		{
		}

		JsonFilterTree(const JsonFilterTree& jTree) : isFreezed(false)
		{
			_name = jTree._name;

			for (auto& node : jTree._filterMap)
			{
				_filterMap[node.first] = node.second;
			}
		}


		void freeze()
		{
			isFreezed = true;
			try
			{
				computeLinkDependencies();
			}
			catch (const HipeException &e)
			{
				throw e;
			}
		}

		///
		/// Add Filter node and insert name of the dependent's parent inside the filter
		/// 
		void add(JsonFilterNode& filterNode)
		{
			auto filter = filterNode.getFilter();
			auto name = filter->getName();

			if (_filterMap.find(name) != _filterMap.end())
			{
				auto errorMessage = std::string("Filter named ");
				errorMessage += name;
				errorMessage += " doesn't exist";

				throw HipeException(errorMessage.c_str());
			}
			_filterMap[name] = filter;

			//Add dependencies name

			for (auto& parentName : filterNode.getDependenciesFilter())
			{
				/*if (parentName == "PerfTime")
				{
					
				}
				else*/
				{
					filter->addDependenciesName(parentName);
				}
			}
		}

		void plugPerfAnalysis(Model* nodeToAnalysis, Model* startPerfTime)
		{
			if (nodeToAnalysis->getParents().find(startPerfTime->getName()) != startPerfTime->getParents().end() &&
				nodeToAnalysis->getParents()[startPerfTime->getName()] != nullptr)
			{
				throw HipeException("Node PerfTime must be linked to one node only for analysis");
			}

			//remove all link to PerfTime object
			startPerfTime->getParents().clear();
			startPerfTime->getChildrens().clear();

			std::map<std::string, Model*> & pairs = nodeToAnalysis->getParents();

			//Remove fake PerfTime parent to the node anlyszed 
			pairs.erase(startPerfTime->getName());

			//For the start timer
			//Now get dependencies of node to analyze and push it to PerfTime
			startPerfTime->getParents() = nodeToAnalysis->getParents();
			for (auto &it : startPerfTime->getParents())
			{
				Model* parent = it.second;
				parent->getChildrens()[startPerfTime->getName()] = startPerfTime;
			}

			//For the end timer
			//Create the EndTimer node
			Model * endPerfTimer = static_cast<filter::Model *>(newFilter("PerfTime"));
			endPerfTimer->setName(nodeToAnalysis->getName() +  "__end_timer___");
			//_filterMap[endPerfTimer->getName()] = endPerfTimer;
			
			endPerfTimer->getParents()[startPerfTime->getName()] = startPerfTime;
			startPerfTime->getChildrens()[endPerfTimer->getName()] = endPerfTimer;

			endPerfTimer->getParents()[nodeToAnalysis->getName()] = nodeToAnalysis;
			nodeToAnalysis->getChildrens()[endPerfTimer->getName()] = endPerfTimer;



			//Push node to analysze to the PerfTime object
			


		}

		//Link every filters between them
		void computeLinkDependencies()
		{
			if (isFreezed == false) throw HipeException("Cannot compute dependencies of tree if the tree is not freezed");

			filter::Model* root = nullptr;

			//Find the root object
			for (auto& it : _filterMap)
			{
				filter::Model* filterNode = it.second;
				const std::string name = it.first;
				if (filterNode->getParents().empty() && 
					filterNode->getConstructorName().find("RootFilter") != std::string::npos)
				{
					root = filterNode;
					break;
				}
			}
			if (root == nullptr)
			{
				root = static_cast<filter::Model *>(newFilter("RootFilter"));
				root->setName("__fake_root___");
				_filterMap[root->getName()] = root;
			}

			for (auto& it : _filterMap)
			{
				filter::Model* filterNode = it.second;
				const std::string name = it.first;

				//Check if there is no parents and it's not a RootFilter
				if (filterNode->getParents().empty() && 
					filterNode->getConstructorName().find("RootFilter") == std::string::npos && 
					filterNode->getConstructorName().find("PerfTime") == std::string::npos) //<--- ignore check perfTime object 
				{
					filterNode->addDependencies(root);
				}
				

				for (auto& parent : filterNode->getParents())
				{
					if (_filterMap[parent.first] == nullptr)
						throw HipeException("No node found with name " + parent.first);

					//Ignore the perftime it has been inserted before
					if (parent.first.find("PerfTime") != std::string::npos)
					{
						plugPerfAnalysis(filterNode, _filterMap[parent.first]);
					}
					else
					{
						filterNode->addDependencies(_filterMap[parent.first]);
					}
				}
			}
			computeLevelNode();
		}

		//Compute the layer inder of the node when it will be executed
		void computeLevelNode()
		{
			if (isFreezed == false) throw HipeException("Cannot compute layer level of tree if the tree is not freezed");

			for (auto& it : _filterMap)
			{
				auto& filter = it.second;
				for (auto& child : filter->getChildrens())
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

		//Find the root
		filter::Model* getRootNode()
		{
			if (isFreezed == false) throw HipeException("Cannot get RootNode without dependencies computation of tree");

			for (auto& it : _filterMap)
			{
				if (it.second->getLevel() == 0)
					return it.second;
			}

			throw HipeException("Unkown error. This code shouldn't happen");
		}

		void addDependencies(Model* filter) override
		{
		}

		void addChildDependencies(Model* filter) override
		{
		}

		void addDependenciesName(std::string filter) override
		{
		}

		void addChildDependenciesName(std::string filter) override
		{
		}
		std::map<std::string, Model*> empty;

		std::map<std::string, Model*> & getParents() override
		{
			empty.clear();
			return empty;
		}

		std::map<std::string, Model*> & getChildrens() override
		{
			empty.clear();
			return empty;
		}
		

		HipeStatus process() override
		{
			return OK;
		}

		Model& operator<<(data::Data& element) override
		{
			return *this;
		}

		Model& operator<<(cv::Mat& element) override
		{
			return *this;
		}

		data::ConnexDataBase& getConnector() override
		{
			throw HipeException("Cannot call method getConnector from the class JsonFilter");
		}
	};
}
