//@HIPE_LICENSE@
#pragma once


#include <string>
#include <iostream>
#include <core/Logger.h>

#pragma warning(push, 0)
#include <boost/property_tree/ptree_fwd.hpp>
#pragma warning(pop)

namespace hipe_engine
{
	class ConfigurationParametersChild
	{
	public:
		std::string aschildproc; //Shared Memory name here
		std::string json_request_file; //Shared Memory name here
		std::string modulePath;
		std::string base_cert;
		bool debugMode;


	public:
		ConfigurationParametersChild();

	private:
		void setDefaultValues();
	};

	class ConfigurationChild
	{
	public:
		ConfigurationParametersChild configuration;


	private:
		std::string configFilePath;

	public:
		ConfigurationChild();
		ConfigurationChild(const std::string& configFilePath);

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
