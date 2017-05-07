#include <tools/RegisterTable.h>
#include <IFilter.h>
#include <stack>
#include <core/Invoker.h>
#include <filter/tools/filterMacros.h>
#include <filter/data/FileImageData.h>
#include <filter/filter_export.h>

FILTER_EXPORT RegisterTable* RegisterTable::instance = nullptr;


void* newFilter(std::string className)
{
	return RegisterTable::getInstance().newObjectInstance(className);
}


filter::IFilter* copyFilter(filter::IFilter* filter)
{
	if (filter == nullptr) return nullptr;
	filter::IFilter* nFilter = RegisterTable::getInstance().newObjectInstance(filter->getConstructorName());
	nFilter->setName(filter->getName());
	nFilter->setLevel(filter->getLevel());

	for (auto& field : getParameterNames(filter->getConstructorName()))
	{
		std::string copyField(field);
		{
			__invoke(nFilter, "copy_" + copyField, filter);
		}
	}
	return nFilter;
}

filter::IFilter* copyAlgorithms(filter::IFilter* root)
{
	if (root == nullptr) return nullptr;
	filter::IFilter* newRoot = copyFilter(root);

	filter::IFilter* copyParent;
	filter::IFilter* parent = root;
	std::stack<filter::IFilter*> heap;
	std::stack<filter::IFilter*> copyHeap;

	heap.push(root);
	copyHeap.push(newRoot);

	while (!heap.empty())
	{
		parent = heap.top();
		copyParent = copyHeap.top();
		heap.pop();
		copyHeap.pop();

		for (auto childMap : parent->getChildrens())
		{
			filter::IFilter* copyChild = copyFilter(childMap.second);
			copyChild->addDependencies(copyParent);
			if (! childMap.second->getChildrens().empty())
			{
				heap.push(childMap.second);
				copyHeap.push(copyChild);
			}
			
		}
	}


	return newRoot;
}

HipeStatus freeAlgorithms(filter::IFilter* root)
{
	if (root == nullptr) return OK;
	
	filter::IFilter* parent = root;
	std::stack<filter::IFilter*> heap;
	std::vector<filter::IFilter*> vectorFilter;

	heap.push(root);
	vectorFilter.push_back(root);

	while (!heap.empty())
	{
		parent = heap.top();
		heap.pop();
		for (auto childMap : parent->getChildrens())
		{
			vectorFilter.push_back(childMap.second);
			if (!childMap.second->getChildrens().empty())
			{
				heap.push(childMap.second);
			}
		}
	}
	for (auto &filter : vectorFilter)
	{
		delete filter;
	}


	return OK;
}




const std::vector<std::string> getTypes(std::string className)
{
	return RegisterTable::getInstance().getMethodNames(className);
}

const std::vector<std::string> getParameterNames(std::string className)
{
	return RegisterTable::getInstance().getVarNames(className);
}
