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

#pragma once
#include "Server.h"
#include "Content.h"

#pragma warning(push, 0)
#include <boost/regex.hpp>
#pragma warning(pop)

#define REGEX_NM boost



namespace http
{
	template <class socket_type>
	class Request
	{

	public:
		//Based on http://www.boost.org/doc/libs/1_60_0/doc/html/unordered/hash_equality.html
		class iequal_to
		{
		public:
			bool operator()(const std::string& key1, const std::string& key2) const
			{
				return boost::algorithm::iequals(key1, key2);
			}
		};

		class ihash
		{
		public:
			size_t operator()(const std::string& key) const
			{
				std::size_t seed = 0;
				for (auto& c : key)
					boost::hash_combine(seed, std::tolower(c));
				return seed;
			}
		};

	public:
		std::string method, path, http_version;

		Content content;

		std::unordered_multimap<std::string, std::string, ihash, iequal_to> header;

		REGEX_NM::smatch path_match;

		std::string remote_endpoint_address;
		unsigned short remote_endpoint_port;

	public:
		Request(const socket_type& socket) : content(streambuf)
		{
			try
			{
				remote_endpoint_address = socket.lowest_layer().remote_endpoint().address().to_string();
				remote_endpoint_port = socket.lowest_layer().remote_endpoint().port();
			}
			catch (...)
			{
			}
		}

		boost::asio::streambuf streambuf;
	};
}
