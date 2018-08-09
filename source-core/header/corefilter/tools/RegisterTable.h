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

#pragma once
#include <map>
#include <vector>
#include <functional>

#include <core/misc.h>
#include <core/HipeStatus.h>
#include <core/Invoker.h>
#include <core/HipeException.h>
#include <algorithm>
#include <sstream>
#include <corefilter/tools/filterMacros.h>
#include <coredata/IOData.h>
#include <coredata/IODataType.h>
#include <corefilter/filter_export.h>
#include <corefilter/Model.h>
#include "json/JsonTree.h"

class FILTER_EXPORT RegisterTable;


FILTER_EXPORT RegisterTable* registerInstance();

extern "C"
{
	FILTER_EXPORT void* c_registerInstance();
}

/**
 * \brief This class register every class and setter functions coming from class inheriting of Model and IFfilter
 */
class FILTER_EXPORT RegisterTable
{
	// Here is the core of the solution: this map of lambdas does all the "magic"
	std::map<std::string, std::function<filter::Model*()>> functionTable;
	std::map<std::string, std::function<filter::Model*()>> functionPrealoadTable;

	std::map<std::string, std::map<std::string, core::InvokerBase>> setterTable;


	std::map<data::IODataType, std::map<std::string, core::InvokerBase>> _IODataTable;

	std::map<filter::Model *, std::string> reverse;


public:

private:

	/**
	 * \brief Private contructor for all reflection
	 */
	RegisterTable()
	{
		
	}

	


public:
	friend RegisterTable* registerInstance();
	/**
	 * \brief Get only a instance of RegisterTable
	 * \return the single instance of RegisterTable
	 */
	static RegisterTable& getInstance()
	{
		return *registerInstance();
	}

public:

	/**
	 * \brief Add class to reflection mapping Key = classname ; value = Contructor method
	 * \param className the class name of the class to register
	 * \param constructor the method to delegate a new operator
	 * \return the class name to be called outside function
	 */
	const std::string addClass(std::string className, std::function<filter::Model*()> constructor)
	{
		if (functionTable[className] == nullptr)
			functionTable[className] = constructor;

		return className;
	}


	/**
	* \brief Add class to reflection mapping Key = classname ; value = Contructor method
	* \param className the class name of the class to register
	* \param constructor the method to delegate a new operator
	* \return the class name to be called outside function
	*/
	const std::string addPreloadClass(std::string className, std::function<filter::Model*()> constructor)
	{
		if (functionPrealoadTable[className] == nullptr)
			functionPrealoadTable[className] = constructor;

		return className;
	}

	/**
	 * \brief Add setter to set field by reflection from any other language
	 * \param classname 
	 * \param functionName 
	 * \param method 
	 * \return 
	 */
	std::string addSetter(const std::string& classname, const std::string& functionName,
	                      core::InvokerBase method)
	{
		setterTable[std::string(classname)][functionName] = method;

		return std::string(classname + "::" + functionName);
	}

	std::vector<std::string> getTypeNames()
	{
		std::vector<std::string> typeNames;

		for (auto& pairs : setterTable)
		{
			typeNames.push_back(pairs.first);
		}

		return typeNames;
	}


	std::vector<std::string> getMethodNames(const std::string& classname)
	{
		std::vector<std::string> methodsNames;

		for (auto& pairs : setterTable[classname])
		{
			methodsNames.push_back(pairs.first);
		}

		return methodsNames;
	}

	const std::vector<std::string> getVarNames(const std::string& classname)
	{
		std::vector<std::string> varNames;

		for (auto& pairs : setterTable[classname])
		{
			if (pairs.first.find("_from_json") == std::string::npos)
				if (pairs.first.find("set_") == 0)
					varNames.push_back(std::string(pairs.first).erase(0, 4)); // remove prefix "set_"
		}

		return varNames;
	}

	const std::string getNamespace(std::string className)
	{
		if (functionPrealoadTable.find(className) == functionPrealoadTable.end())
		{
			return "UNKNOWN_ISSUE";
		}

		filter::Model* ret = functionPrealoadTable[className]();

		std::string _namespace = ret->getNamespace();

		return _namespace;
	}


	void getVariable(json::JsonTree & jsonNode, filter::Model *filter, std::string fieldName)
	{
		if (setterTable.find(filter->getConstructorName()) == setterTable.end())
		{
			throw HipeException("Cannot get Variable by instance with type " + filter->getConstructorName());
		}
		

		invoke(filter, "get_json_" + fieldName, jsonNode);

	}

	template <typename...Args>
	void invoke(filter::Model* instance, std::string functionName, Args ...args)
	{
		std::string typeStr = functionName;

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("set") != std::string::npos || typeStr.find("copy") != std::string::npos || typeStr.find("get_json") != std::string::npos)
		{
			core::InvokerBase d = setterTable[reverse[instance]][functionName];
			d.operator()<void, Args...>(instance, args...);
		}
		else
			throw HipeException("TODO : Don't know how to manage getter method");
	}

	filter::Model* newObjectInstance(std::string className, bool managed = true)
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

};


FILTER_EXPORT void* newFilter(std::string className);

FILTER_EXPORT const std::vector<std::string> getTypes(std::string className);

FILTER_EXPORT const std::vector<std::string> getParameterNames(std::string className);

FILTER_EXPORT filter::Model* copyFilter(filter::Model* filter);

FILTER_EXPORT filter::Model* copyAlgorithms(filter::Model* root);

FILTER_EXPORT HipeStatus freeAlgorithms(filter::Model* root);


