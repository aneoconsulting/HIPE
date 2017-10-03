#pragma once


#include <string>
#include <iostream>
#include <core/Logger.h>
namespace hipe_server
{
	class ConfigurationParameters
	{
	public:
		unsigned short port;


	public:
		ConfigurationParameters();

	private:
		void setDefaultValues();
	};

	class Configuration
	{
	public:
		static core::Logger configLogger;

		ConfigurationParameters configuration;


	private:
		//std::string configFilePath;


	public:
		Configuration();
		//Configuration(const std::string& configFilePath);

		int setConfigFromCommandLine(int argc, char* argv[]);


		//std::string getConfigFilePath();
		//std::string getConfigFileName();
		//std::string getConfigFileDir();


	private:


		//void updateConfiguration();
		//template<class T>
		//T getValue(boost::property_tree::ptree node, std::string elem);
	};
}