#include <core/StringTools.h>
#include <algorithm>

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

std::string HexAsString(unsigned long value, int base, int minSize = 1)
{
	static char const digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	std::string results;
	while (value != 0 || results.size() < minSize) {
		results += digits[value % base];
		value /= base;
	}
	std::reverse(results.begin(), results.end());
	return results;
}

std::string ToString(long n)
{

	std::string hex = HexAsString(n, 16);

	int len = hex.length();
	std::string newString;
	for (int i = 0; i< len; i += 2)
	{
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), nullptr, 16);
		newString.push_back(chr);
	}
	return newString;
}
