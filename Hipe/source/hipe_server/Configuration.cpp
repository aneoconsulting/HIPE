#include <hipe_server/Configuration.h>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>


namespace bpo = boost::program_options;

namespace hipe_server
{
	core::Logger Configuration::configLogger = core::setClassNameAttribute("Configuration");

	ConfigurationParameters::ConfigurationParameters()
	{
		setDefaultValues();
	}

	void ConfigurationParameters::setDefaultValues()
	{
		this->port = 8080;
	}

	Configuration::Configuration() /*: configFilePath("UNNASSIGNED")*/
	{
	}

	//Configuration::Configuration(const std::string& configFilePath) : configFilePath(configFilePath)
	//{
	//}

	int Configuration::setConfigFromCommandLine(int argc, char* argv[])
	{
		// General options
		bpo::options_description generalCat("General");
		generalCat.add_options()
			("help", "Displays this dialog")
			("help-module", bpo::value<std::string>(), "Displays the help page related to a given sub module")
			;
		// Configuration
		bpo::options_description configCat("Configuration");
		configCat.add_options()
			("port,p", bpo::value<unsigned short>(&this->configuration.port)->default_value(8080), "Sets the port the server should be listening on")
			;

		// Regroup all sub catagories
		bpo::options_description allCat("Available options");
		allCat.add(generalCat).add(configCat);

		// Regroup all categories options that should be displayed to the user 
		bpo::options_description visibleCat("User available options");
		visibleCat.add(generalCat).add(configCat);

		// Parse command line
		bpo::variables_map vm;
		bpo::store(bpo::parse_command_line(argc, argv, allCat), vm);
		bpo::notify(vm);

		// Handle custom options
		// Help command
		if (vm.count("help"))
		{
			std::cout << visibleCat;
			return 1;
		}

		//Help module command
		if (vm.count("help-module"))
		{
			const std::string& module = vm["help-module"].as<std::string>();

			if (module == "configuration" || module == "Configuration")
			{
				std::cout << std::endl << configCat << std::endl;;
			}
			else
			{
				std::cout << std::endl << "unknown module: " << module << std::endl; ;
			}

			return 1;
		}

		configLogger << "port set to " + std::to_string(this->configuration.port);

		return 0;
	}


	//std::string Configuration::getConfigFilePath()
	//{
	//	return this->configFilePath;
	//}

	//std::string Configuration::getConfigFileName()
	//{
	//	std::size_t pos = this->configFilePath.find_last_of("/\\");
	//	return configFilePath.substr(pos + 1);
	//}

	//std::string Configuration::getConfigFileDir()
	//{
	//	std::size_t pos = this->configFilePath.find_last_of("/\\");
	//	return configFilePath.substr(0, pos);
	//}


	//void Configuration::updateConfiguration()
	//{
	//	boost::property_tree::ptree ptree;
	//	boost::property_tree::read_json(filePath, ptree);
	//
	//	if (ptree.count("http") != 0)
	//	{
	//		auto httpNode = ptree.get_child("http");
	//		configuration.port = getValue<int>(httpNode, "port");
	//
	//	}
	//	std::cout << "test" << std::endl;
	//}

	//template <class T>
	//T Configuration::getValue(boost::property_tree::ptree node, std::string elem)
	//{
	//	if (node.count(elem) != 0) return node.get<T>(elem);
	//	return T();
	//}
}