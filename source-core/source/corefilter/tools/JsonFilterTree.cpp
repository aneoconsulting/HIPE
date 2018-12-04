#include <corefilter/tools/JsonFilterNode/JsonFilterTree.h>


json::JsonFilterTree::JsonFilterTree(): isFreezed(false), perftimerBuilder(true)
{
	
}

json::JsonFilterTree::JsonFilterTree(const JsonFilterTree& jTree): isFreezed(false), perftimerBuilder(true)
{
	_name = jTree._name;

	for (auto& node : jTree._filterMap)
	{
		_filterMap[node.first] = node.second;
	}
}

void json::JsonFilterTree::freeze()
{
	isFreezed = true;
	try
	{
		computeLinkDependencies();
	}
	catch (const HipeException& e)
	{
		throw e;
	}
}

void json::JsonFilterTree::add(JsonFilterNode& filterNode)
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
		if (this->needPerfTimeBuilder() && parentName.find("PerfTime") != std::string::npos)
		{
			//Add node end timer to encasuplate the node to analyze
			Model* endPerfTimer = static_cast<filter::Model *>(newFilter("PerfTime"));
			_filterMap[name + "__end_timer___"] = endPerfTimer;
			endPerfTimer->setName(name + "__end_timer___");
			endPerfTimer->addDependenciesName(name);
			filter->addDependenciesName(parentName);
			//endPerfTimer->addDependenciesName(parentName);
		}
		else
		{
			filter->addDependenciesName(parentName);
		}
	}
}

void json::JsonFilterTree::plugPerfAnalysis(Model* nodeToAnalysis, Model* startPerfTime)
{
	std::map<std::string, Model *> newFilterMap;

	std::map<std::string, Model*>& pairs = startPerfTime->getParents();

	std::map<std::string, Model*> nodeToAnalysis_pairs = nodeToAnalysis->getParents();

	if (nodeToAnalysis_pairs.find(startPerfTime->getName()) != nodeToAnalysis_pairs.end() &&
		nodeToAnalysis_pairs[startPerfTime->getName()] != nullptr)
	{
		throw HipeException("Node PerfTime must be linked to one node only for analysis");
	}

	//remove all link to PerfTime object
	pairs.clear();
	startPerfTime->getChildrens().clear();


	//Remove fake PerfTime parent to the node anlyszed 
	nodeToAnalysis_pairs.erase(startPerfTime->getName());

	//For the start timer
	//Now get dependencies of node to analyze and push it to PerfTime

	for (auto& it : nodeToAnalysis_pairs)
	{
		Model* parent = it.second;
		parent->getChildrens()[startPerfTime->getName()] = startPerfTime;
	}

	//For the end timer

	Model* endPerfTimer = _filterMap[nodeToAnalysis->getName() + "__end_timer___"];
	endPerfTimer->getParents()[startPerfTime->getName()] = startPerfTime;
	startPerfTime->getChildrens()[endPerfTimer->getName()] = endPerfTimer;

	endPerfTimer->getParents()[nodeToAnalysis->getName()] = nodeToAnalysis;
	nodeToAnalysis->getChildrens()[endPerfTimer->getName()] = endPerfTimer;


	//Push node to analysze to the PerfTime object
}

void json::JsonFilterTree::computeLinkDependencies()
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

		bool needKPI = false;
		

		for (auto& parent : filterNode->getParents())
		{
			if (_filterMap[parent.first] == nullptr)
				throw HipeException("No node found with name " + parent.first);

			//Ignore the perftime it has been inserted before
			//And avoid to find the end timer that we have created before
			if (this->needPerfTimeBuilder() && parent.first.find("PerfTime") != std::string::npos && filterNode->getName().find("__end_timer___") == std::string::npos)
			{
				//A perftime has been found, let link all others parents to the node before
				needKPI = true;
			}
			else
			{
				filterNode->addDependencies(_filterMap[parent.first]);
			}
		}

		//If we found KPI PerfTime links have to updated and plugged around the node
		if (needKPI)
		{
			for (auto& parent : filterNode->getParents())
			{
				if (parent.first.find("PerfTime") != std::string::npos && filterNode->getName().find("__end_timer___") == std::string::npos)
				{
					plugPerfAnalysis(filterNode, _filterMap[parent.first]);
				}
			}
		}
	}
	computeLevelNode();
}

void json::JsonFilterTree::computeLevelNode()
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

filter::Model* json::JsonFilterTree::getRootNode()
{
	if (isFreezed == false) throw HipeException("Cannot get RootNode without dependencies computation of tree");

	for (auto& it : _filterMap)
	{
		if (it.second->getLevel() == 0)
			return it.second;
	}

	throw HipeException("Unkown error. This code shouldn't happen");
}

void json::JsonFilterTree::addDependencies(Model* filter)
{
}

void json::JsonFilterTree::addChildDependencies(Model* filter)
{
}

void json::JsonFilterTree::addDependenciesName(std::string filter)
{
}

void json::JsonFilterTree::addChildDependenciesName(std::string filter)
{
}

std::map<std::string, filter::Model*>& json::JsonFilterTree::getParents()
{
	empty.clear();
	return empty;
}

std::map<std::string, filter::Model*>& json::JsonFilterTree::getChildrens()
{
	empty.clear();
	return empty;
}

HipeStatus json::JsonFilterTree::process()
{
	return OK;
}

filter::Model& json::JsonFilterTree::operator<<(data::Data& element)
{
	return *this;
}

filter::Model& json::JsonFilterTree::operator<<(cv::Mat& element)
{
	return *this;
}

data::ConnexDataBase& json::JsonFilterTree::getConnector()
{
	throw HipeException("Cannot call method getConnector from the class JsonFilter");
}
