#pragma once
#include <map>
#include <filter/Model.h>
#include <json/JsonFilterNode/JsonFilterNode.h>
#include <core/HipeException.h>


namespace json
{
	class JsonFilterTree : public filter::Model
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
				throw ;
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
				filter->addDependenciesName(parentName);
			}
		}

		//Link every filters between them
		void computeLinkDependencies()
		{
			if (isFreezed == false) throw HipeException("Cannot compute dependencies of tree if the tree is not freezed");

			for (auto& it : _filterMap)
			{
				filter::Model* filterNode = it.second;
				const std::string name = it.first;

				for (auto& parent : filterNode->getParents())
				{
					if (_filterMap[parent.first] == nullptr)
						throw HipeException("No node found with name " + parent.first);

					filterNode->addDependencies(_filterMap[parent.first]);
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

		std::map<std::string, Model*> getParents() const override
		{
			return std::map<std::string, Model*>();
		}

		std::map<std::string, Model*> getChildrens() const override
		{
			return std::map<std::string, Model*>();
		}

		Model* getRootFilter() override
		{
			return nullptr;
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
