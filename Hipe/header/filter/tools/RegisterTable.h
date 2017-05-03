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
#include "RegisterTools.hpp"
#include "filterMacros.h"

namespace filter {
	class IFilter;
}


/**
 * \brief This class register every class and setter functions coming from class inheriting of Model and IFfilter
 */
class DLL_PUBLIC RegisterTable
{
	// Here is the core of the solution: this map of lambdas does all the "magic"
	std::map<std::string, std::function<filter::IFilter*()> > functionTable;

	std::map<std::string, std::map<std::string, core::InvokerBase >> setterTable;

	std::map<filter::IFilter *, std::string> reverse;


public:
	
private:

	/**
	 * \brief Private contructor for all reflection
	 */
	RegisterTable()
	{
		
	}

	static RegisterTable *instance;


public:

	/**
	 * \brief Get only a instance of RegisterTable
	 * \return the single instance of RegisterTable
	 */
	static RegisterTable& getInstance()
	{
		if (instance == nullptr)
		{
			instance = new RegisterTable();
		}
		return *instance;
	}

public:

	/**
	 * \brief Add class to reflection mapping Key = classname ; value = Contructor method
	 * \param className the class name of the class to register
	 * \param constructor the method to delegate a new operator
	 * \return the class name to be called outside function
	 */
	const std::string addClass(std::string className, std::function<filter::IFilter*()> constructor)
	{
		if (functionTable[className] == nullptr) 
			functionTable[className] = constructor;

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

		for (auto & pairs : setterTable)
		{
			typeNames.push_back(pairs.first);
		}

		return typeNames;
	}


	std::vector<std::string> getMethodNames(const std::string& classname)
	{
		std::vector<std::string> methodsNames;

		for (auto & pairs : setterTable[classname])
		{
			methodsNames.push_back(pairs.first);
		}

		return methodsNames;
	}

	const std::vector<std::string> getVarNames(const std::string& classname)
	{
		std::vector<std::string> varNames;

		for (auto & pairs : setterTable[classname])
		{
			if (pairs.first.find("_from_json") == std::string::npos)
				if (pairs.first.find("set_") == 0)
					varNames.push_back(std::string(pairs.first).erase(0, 4)); // remove prefix "set_"

		}
		
		return varNames;
	}

	template<typename...Args>
	void invoke(filter::IFilter * instance, std::string functionName, Args...args)
	{
		std::string typeStr = functionName;

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("set") != std::string::npos || typeStr.find("copy") != std::string::npos)
		{
			core::InvokerBase d = setterTable[reverse[instance]][functionName];
			d.operator() < void, Args... > (instance, args...);
		}
		else
			throw HipeException("TODO : Don't know how to manage getter method");
	}

	filter::IFilter *newObjectInstance(std::string className, bool managed = true)
	{
		std::function<filter::IFilter*()> function = functionTable[className];

		if (!function)
		{
			std::stringstream build_string;
			build_string << "the constructor of class " << className << " doesn't exist (" << TO_STR(FILE_BASENAME) << ":" << __LINE__ << ")";

			throw HipeException(build_string.str());
		}
			
		filter::IFilter *ret = functionTable[className]();

		//if (managed)
			reverse[ret] = className;

		return ret;
	}

};



DLL_PUBLIC void * newFilter(std::string className);

DLL_PUBLIC const std::vector<std::string> getTypes(std::string className);

DLL_PUBLIC const std::vector<std::string> getParameterNames(std::string className);

DLL_PUBLIC filter::IFilter* copyFilter(filter::IFilter* filter);

DLL_PUBLIC filter::IFilter* copyAlgorithms(filter::IFilter* root);

DLL_PUBLIC HipeStatus freeAlgorithms(filter::IFilter* root);