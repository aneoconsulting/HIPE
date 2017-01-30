#pragma once
#include <map>
#include <vector>
#include <functional>
#include "functor.hpp"


namespace filter {
	class IFilter;
}

class RegisterTable
{
	// Here is the core of the solution: this map of lambdas does all the "magic"
	std::map<std::string, std::function<filter::IFilter*()> > functionTable;

	std::map<std::string, std::map<std::string, dtFunctor >> setterTable;

	std::map<filter::IFilter *, std::string> reverse;


public:
	//template <typename... Args>
	//wrapped<Args...> & getWrapper(std::function<void(Args...args)>)
	//{
	//	wrapped<Args...>
	//}

private:
	RegisterTable()
	{
		
	}

	static RegisterTable *instance;


public:

	static RegisterTable& getInstance()
	{
		if (instance == nullptr)
		{
			instance = new RegisterTable();
		}
		return *instance;
	}

public:
	const std::string addClass(std::string className, std::function<filter::IFilter*()> constructor)
	{
		if (functionTable[className] == nullptr) 
			functionTable[className] = constructor;

		return className;
	}

	const std::string addSetter(const std::string& classname, const std::string& functionName,
		dtFunctor & method)
	{
		
		//if (setterTable[classname + "::" + functionName] == 0)


		//method((functionTable[classname]()), 5);
		setterTable[std::string(classname)][functionName] = method;
		//setterTable.

		return std::string(classname + "::" + functionName);
	}

	const std::vector<std::string> getMethodNames(const std::string& classname)
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

		(setterTable[reverse[instance]])[functionName](instance, args...);
	}

	filter::IFilter *newObjectInstance(std::string className)
	{
		filter::IFilter *ret = functionTable[className]();

		reverse[ret] = className;

		return ret;
	}

};

#define newFilter(className) RegisterTable::getInstance().newObjectInstance(className)

#define __invoke(instance, function,...) RegisterTable::getInstance().invoke(instance, function,  __VA_ARGS__)
#define getTypes(className) RegisterTable::getInstance().getMethodNames(className)
#define getParameterNames(className) RegisterTable::getInstance().getVarNames(className)

