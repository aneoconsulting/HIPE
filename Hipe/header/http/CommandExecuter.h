#pragma once
#include <iostream>
#include "orchestrator/Orchestrator.h"

namespace http
{
	class CommandExecuter {
	public :
		static	bool kill_command_executer(std::string optionName, boost::property_tree::ptree * ltreeResponse)
		{
			if (optionName.compare("kill") == 0) //compare if optionName == kill otherwise return false;
			{
				orchestrator::OrchestratorFactory::getInstance()->killall();
				ltreeResponse->add("Status", "Task has been killed");
				return true;
			}

			return false;
		}
		static	bool exit_command_executer(std::string optionName, boost::property_tree::ptree *ltreeResponse)
		{
			const std::string exit = "exit";
			if(exit.find(optionName))
			{
				orchestrator::OrchestratorFactory::getInstance()->killall();
				ltreeResponse->add("Status", "Task has been killed");
				ltreeResponse->add("process", "Server is exiting");
				return true;
			}
			return false;
		}
		static bool get_server_info(std::string optionName)
		{
			// chdoisir un nom cool pour la commande de récupération d'info server
			return false;
		}
	};
}
