#include <CommandExecuter.h>

bool http::CommandExecuter::kill_command_executer(std::string optionName, boost::property_tree::ptree* ltreeResponse)
{
	if (optionName.compare("kill") == 0) //compare if optionName == kill otherwise return false;
	{
		orchestrator::OrchestratorFactory::getInstance()->killall();
		ltreeResponse->add("Status", "Task has been killed");
		return true;
	}

	return false;
}
	bool http::CommandExecuter::exit_command_executer(std::string optionName, boost::property_tree::ptree *ltreeResponse)
{
	const std::string exit = "exit";
	if (exit.find(optionName) == 0)
	{
		orchestrator::OrchestratorFactory::getInstance()->killall();
		ltreeResponse->add("Status", "Task has been killed");
		ltreeResponse->add("process", "Server is exiting");
		return true;
	}
	return false;
}

 bool http::CommandExecuter::get_filters(std::string optionName, boost::property_tree::ptree *ltreeResponse)
{
	const std::string filters = "filters";
	int i = 0;
	if (filters.find(optionName) == 0)
	{
		RegisterTable & reg = RegisterTable::getInstance();
		for (auto &name : reg.getTypeNames())
		{
			boost::property_tree::ptree parameters;
			boost::property_tree::ptree child;
			for (auto &varName : reg.getVarNames(name))
			{
				child.put(varName, "");
			}

			parameters.push_back(std::make_pair("", child));
			ltreeResponse->add_child(name, parameters);
			++i;
		}
		return true;
	}
	return false;
}

 bool http::CommandExecuter::get_version(std::string optionName, boost::property_tree::ptree *ltreeResponse)
 {
	 const std::string version = "version";
	 if (version.find(optionName) == 0)
	 {
		 return true;
	 }
	 return false;
 }