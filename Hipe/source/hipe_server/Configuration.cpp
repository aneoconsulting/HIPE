#include <hipe_server/Configuration.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>
#include <core/HipeException.h>


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
		this->modulePath = "";
	}

	Configuration::Configuration()
	{
	}

	Configuration::Configuration(const std::string& configFilePath) : configFilePath(configFilePath)
	{
	}

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
			("port,p", bpo::value<unsigned short>(&this->configuration.port)->default_value(this->configuration.port), "Sets the port the server should be listening on")
			;
		configCat.add_options()
			("module,m", bpo::value<std::string>(&this->configuration.modulePath)->default_value(this->configuration.modulePath), "Sets the path to the module to get all filters implemented")
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
		return 0;
	}

	int Configuration::setConfigFromFile()
	{
		boost::property_tree::ptree configPtree;
		try
		{
			boost::property_tree::read_json(this->configFilePath, configPtree);
		}
		catch (const std::exception& e)
		{
			this->configLogger << "Couldn't find configuration file.";
			return 1;
		}


		if (configPtree.count("http") != 0)
		{
			auto httpNode = configPtree.get_child("http");
			configuration.port = getValue<unsigned short>(httpNode, "port");
		}

		return 0;
	}

	int Configuration::setConfigFromFile(const std::string& filePath)
	{
		this->configFilePath = filePath;
		return setConfigFromFile();
	}

	void Configuration::displayConfig() const
	{
		configLogger << "port set to " + std::to_string(this->configuration.port);
	}


	std::string Configuration::getConfigFilePath() const
	{
		return this->configFilePath;
	}

	std::string Configuration::getConfigFileName() const
	{
		std::size_t pos = this->configFilePath.find_last_of("/\\");
		return configFilePath.substr(pos + 1);
	}

	std::string Configuration::getConfigFileDir() const
	{
		std::size_t pos = this->configFilePath.find_last_of("/\\");
		return configFilePath.substr(0, pos);
	}

	template <class T>
	T Configuration::getValue(boost::property_tree::ptree node, std::string elem) const
	{
		if (node.count(elem) != 0) return node.get<T>(elem);
		return T();
	}
}