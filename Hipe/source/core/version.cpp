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

std::string getHash()
{
	auto version = getVersion();
	version.erase(std::remove(version.begin(), version.end(), '.'), version.end());
	version.erase(std::remove(version.begin(), version.end(), '_'), version.end());
	version.erase(std::remove(version.begin(), version.end(), '-'), version.end());
	return version;
}