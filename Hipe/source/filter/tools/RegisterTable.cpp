#include <tools/RegisterTable.h>


DLL_PUBLIC  RegisterTable * RegisterTable::instance = nullptr;



void * newFilter(std::string className)
{
	return RegisterTable::getInstance().newObjectInstance(className);
}


const std::vector<std::string> getTypes(std::string className)
{
	return RegisterTable::getInstance().getMethodNames(className);
}

const std::vector<std::string> getParameterNames(std::string className)
{
	return RegisterTable::getInstance().getVarNames(className);
}
