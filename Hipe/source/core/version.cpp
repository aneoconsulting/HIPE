#include <core/version.h>
#include <string>
#include <sstream>  
#include <algorithm>


std::string getVersion()
{
	std::stringstream stream;

	stream << MAJOR_VERSION << "." << MINOR_VERSION;
	stream << "-" << NEXT_MAJOR << NEXT_MINOR;

	stream << "_" << BUILD_VERSION;

	return stream.str();
}

std::string getVersionHashed()
{
	std::string version = getVersion();
	std::string versionHashed;

	for (char c : version)
	{
		if (isdigit(c))	versionHashed += c;
	}

	//return std::regex_replace(getVersion(), std::regex(R"([^0-9])"), "");
	return versionHashed;

}
