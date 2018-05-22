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
