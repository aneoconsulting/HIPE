#include <filter/tools/StringTools.h>


long ToHex(const std::string& s, bool upper_case)
{

	std::ostringstream ret;

	for (std::string::size_type i = 0; i < s.length(); ++i)
		ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << static_cast<int>(s[i]);

	unsigned long value;
	std::istringstream iss(ret.str());

	iss >> std::hex >> value;

	return value;
}
