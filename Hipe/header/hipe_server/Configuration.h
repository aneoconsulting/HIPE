#pragma once


#include <string>
#include <iostream>
#include <core/Logger.h>
#include <boost/property_tree/ptree_fwd.hpp>

namespace hipe_server
{
	class ConfigurationParameters
	{
	public:
		unsigned short port;
		std::string modulePath;


	public:
		ConfigurationParameters();

	private:
		void setDefaultValues();
	};

	class Configuration
	{
	public:
		ConfigurationParameters configuration;


	private:
		std::string configFilePath;
		static core::Logger configLogger;


	public:
		Configuration();
		Configuration(const std::string& configFilePath);

		int setConfigFromCommandLine(int argc, char* argv[]);

		int setConfigFromFile();
		int setConfigFromFile(const std::string & filePath);

		void displayConfig() const;

		std::string getConfigFilePath() const;
		std::string getConfigFileName() const;
		std::string getConfigFileDir() const;


	private:

		void updateConfiguration();
		template<class T>
		T getValue(boost::property_tree::ptree node, std::string elem) const;
	};
}
