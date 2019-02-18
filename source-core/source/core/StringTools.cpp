//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <core/StringTools.h>
#include <algorithm>


/**
 * \brief Convert String to hexa code
 * \param s the string to convert
 * \param upper_case check if it's case sensitive
 * \return the value in a 64 bit int format
 */
hUInt64 ToHex(const std::string& s, bool upper_case)
{

	std::stringstream ret;

	for (std::string::size_type i = 0; i < s.length(); ++i)
		ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << static_cast<hUInt64>(s[i]);


	hUInt64 value;
	std::stringstream iss;
	value = strtoull(ret.str().c_str(), NULL, 16);
	

	return value;
}

std::string HexAsString(hUInt64 value, int base, int minSize = 1)
{
	static char const digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	std::string results;
	while (value != 0 || results.size() < (unsigned int) minSize) {
		results += digits[value % base];
		value /= base;
	}
	std::reverse(results.begin(), results.end());
	return results;
}

std::string ToString(hUInt64 n)
{

	std::string hex = HexAsString(n, 16);

	int len = hex.length();
	std::string newString;
	for (int i = 0; i< len; i += 2)
	{
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtoull(byte.c_str(), nullptr, 16);
		newString.push_back(chr);
	}
	return newString;
}


std::string removeDirectoryName(std::string pathname)
{
	std::string sep;
#ifdef WIN32
	sep = "\\";
#else
	sep = "/";
#endif
	if (pathname.find("\\") == -1 && pathname.find("/") == -1)
		return pathname;

	return pathname.substr(pathname.find_last_of("/\\") + 1, pathname.size());

}

std::string extractDirectoryName(const std::string& str)
{
	size_t found;
	
	found = str.find_last_of("/\\");
	return str.substr(0, found);
	
}

std::string removeFileExtension(const std::string&str)
{
	size_t found;

	found = str.find_last_of(".");
	return str.substr(0, found);
}

std::string extractFileName(const std::string&str)
{
	std::string tmp = removeDirectoryName(str);
	tmp = removeFileExtension(tmp);

	return tmp;
}
