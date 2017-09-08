#pragma once
#include <iostream>
#include "orchestrator/Orchestrator.h"

namespace http
{
	class CommandExecuter {
	public :
		static	bool kill_command_executer(std::string optionName, boost::property_tree::ptree * ltreeResponse);
		static	bool exit_command_executer(std::string optionName, boost::property_tree::ptree *ltreeResponse);

		static bool get_filters(std::string optionName, boost::property_tree::ptree *ltreeResponse);
		static bool get_version(std::string optionName, boost::property_tree::ptree *ltreeResponse);
	};
}
