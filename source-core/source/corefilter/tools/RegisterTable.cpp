//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <corefilter/tools/RegisterTable.h>
#include <corefilter/Model.h>
#include <stack>
#include <core/Invoker.h>
#include <corefilter/tools/filterMacros.h>

#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <mutex>

RegisterTable* registerInstance_l = nullptr;

extern "C"
{
	void* c_registerInstance()
	{
		return (void *)registerInstance();
	}
}

const std::string RegisterTable::getDefaultValue(std::string className, std::string fieldName)
{
	filter::Model* ret = newObjectInstance(className);


	json::JsonTree jsonNode;

	getVariable(jsonNode, ret, fieldName);
	std::string value = jsonNode.get<std::string>(fieldName);

	reverse.erase(ret);
	delete ret;

	return value;
}

void RegisterTable::getVariable(json::JsonTree& jsonNode, filter::Model* filter, std::string fieldName)
{
	if (setterTable.find(filter->getConstructorName()) == setterTable.end())
	{
		throw HipeException("Cannot get Variable by instance with type " + filter->getConstructorName());
	}
	if (fieldName == "unused")
		jsonNode.put<int>(std::string ("unused"), 0);
	else
		invoke(filter, "get_json_" + fieldName, jsonNode);
}

filter::Model* RegisterTable::newObjectInstance(std::string className, bool managed)
{
	std::function<filter::Model*()> function = functionTable[className];

	if (!function)
	{
		std::stringstream build_string;
		build_string << "the constructor of class " << className << " doesn't exist (" << TO_STR(FILE_BASENAME) << ":" << __LINE__ << ")";

		throw HipeException(build_string.str());
	}

	filter::Model* ret = functionTable[className]();

	//if (managed)
	reverse[ret] = className;

	return ret;
}

void* newFilter(std::string className)
{
	return RegisterTable::getInstance().newObjectInstance(className);
}


filter::Model* copyFilter(filter::Model* filter)
{
	if (filter == nullptr) return nullptr;
	filter::Model* nFilter = RegisterTable::getInstance().newObjectInstance(filter->getConstructorName());
	nFilter->setName(filter->getName());
	nFilter->setLevel(filter->getLevel());

	for (auto& field : getParameterNames(filter->getConstructorName()))
	{
		std::string copyField(field);
		{
			__callfunc(nFilter, "copy_" + copyField, filter);
		}
	}
	return nFilter;
}

filter::Model* copyAlgorithms(filter::Model* root)
{
	if (root == nullptr) return nullptr;
	filter::Model* newRoot = copyFilter(root);

	filter::Model* copyParent;
	filter::Model* parent = static_cast<filter::IFilter*>(root);

	std::stack<filter::Model*> heap;
	std::stack<filter::Model*> copyHeap;

	heap.push(parent);
	copyHeap.push((filter::IFilter*)newRoot);
	std::map<std::string, filter::Model *> copyChildCache; //To avoid multiple instance of same object with the same name;

	while (!heap.empty())
	{
		parent = heap.top();
		copyParent = copyHeap.top();
		heap.pop();
		copyHeap.pop();

		for (auto childMap : parent->getChildrens())
		{
			filter::Model* copyChild = nullptr;
			if (copyChildCache.find(childMap.second->getName()) != copyChildCache.end())
			{
				copyChild = copyChildCache[childMap.second->getName()];
			}
			else
			{
				copyChild = static_cast<filter::Model*>(copyFilter(childMap.second));
				copyChildCache[childMap.second->getName()] = copyChild;

			}
			copyChild->addDependencies(copyParent);

			if (! childMap.second->getChildrens().empty())
			{
				heap.push(static_cast<filter::IFilter*>(childMap.second));
				copyHeap.push(copyChild);
			}

		}
	}


	return newRoot;
}



void updateFilter(filter::Model* src, filter::Model* dest)
{
	if (src == nullptr) return;
	filter::Model* nFilter = dest;
	nFilter->setName(src->getName());
	nFilter->setLevel(src->getLevel());

	for (auto& field : getParameterNames(src->getConstructorName()))
	{
		std::string copyField(field);
		{
			__callfunc(nFilter, "copy_" + copyField, src);
		}
	}
	return ;
}

void updateParameters(filter::Model* src, filter::Model* dest)
{
	if (src == nullptr) return ;
	updateFilter(src, dest);

	filter::Model* destParent = static_cast<filter::IFilter*>(dest);;
	filter::Model* srcParent = static_cast<filter::IFilter*>(src);

	std::stack<filter::Model*> heap;
	std::stack<filter::Model*> destHeap;

	heap.push(srcParent);
	destHeap.push(destParent);
	std::map<std::string, filter::Model *> destNodeCache; //To avoid multiple instance of same object with the same name;
	std::map<std::string, filter::Model *> srcNodeCache; //To avoid multiple instance of same object with the same name;

	//First step get all nodes from source 
	while (!heap.empty())
	{
		srcParent = heap.top();
		heap.pop();

		for (auto srcNodeMap : srcParent->getChildrens())
		{
			if (srcNodeCache.find(srcNodeMap.second->getName()) == srcNodeCache.end())
			{
				srcNodeCache[srcNodeMap.second->getName()] = srcNodeMap.second;
			}
			//destChild->addDependencies(destParent);

			if (!srcNodeMap.second->getChildrens().empty())
			{
				heap.push(static_cast<filter::IFilter*>(srcNodeMap.second));
			}
		}
	}

	//Second step get all nodes from destination 
	while (!destHeap.empty())
	{
		destParent = destHeap.top();
		destHeap.pop();
		for (auto destNodedMap : destParent->getChildrens())
		{
			if (destNodeCache.find(destNodedMap.second->getName()) == destNodeCache.end())
			{
				destNodeCache[destNodedMap.second->getName()] = destNodedMap.second;
			}
			//destChild->addDependencies(destParent);
			
			if (!destNodedMap.second->getChildrens().empty())
			{
				destHeap.push(static_cast<filter::IFilter*>(destNodedMap.second));
			}
		}
	}
	std::map<std::string, filter::Model *> newDestNodeCache;

	for (auto srcNodeMap : srcNodeCache)
	{
		//Easier step update parameter of node existing in src and dest
		if (destNodeCache.find(srcNodeMap.second->getName()) != destNodeCache.end())
		{
			updateFilter(srcNodeMap.second, destNodeCache[srcNodeMap.second->getName()]);
			destNodeCache.erase(srcNodeMap.second->getName());
		}
		// Node doesn't exist in dest then create new one by copy in dest and add dependencies
		else 
		{  //TODO: Instanciate when update will take into account the add/remove of node
			newDestNodeCache[srcNodeMap.second->getName()] = static_cast<filter::Model*>(copyFilter(srcNodeMap.second));
		}
	}

	//Third step remove all destNode that doesn't exist in srcNodeCache
	if(!destNodeCache.empty() || ! newDestNodeCache.empty())
	{
		//TODO It's an hard stuff to do here. Need Signal Processing that the tree has changed.
		//TOPO For now Resend is best solution
		throw HipeException("The update contains remove and add filter. It's not yet implemented. Please Resend rather than update");
	}
	

	return ;
}

HipeStatus freeAlgorithms(filter::Model* root)
{
	if (root == nullptr) return OK;

	filter::Model* parent = static_cast<filter::IFilter*>(root);
	std::stack<filter::Model*> heap;
	std::map<filter::Model*, int> cacheFilter;

	heap.push(parent);
	cacheFilter[parent] = 0;;

	while (!heap.empty())
	{
		parent = heap.top();
		heap.pop();
		for (auto childMap : parent->getChildrens())
		{
			cacheFilter[childMap.second] = 0;
			if (!childMap.second->getChildrens().empty())
			{
				heap.push(childMap.second);
			}
		}
	}
	for (auto &filter : cacheFilter)
	{
		delete filter.first;
	}

	cacheFilter.clear();

	return OK;
}

RegisterTable* registerInstance()
{
	static std::mutex _mutex;

	_mutex.lock();
	if (registerInstance_l == nullptr)
	{
		registerInstance_l = new RegisterTable();
	}
	_mutex.unlock();

	return registerInstance_l;
}


const std::vector<std::string> getTypes(std::string className)
{
	return RegisterTable::getInstance().getMethodNames(className);
}

const std::vector<std::string> getParameterNames(std::string className)
{
	return RegisterTable::getInstance().getVarNames(className);
}
