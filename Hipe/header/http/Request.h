#pragma once
#include "Server.h"
#include "Content.h"

#include <boost/regex.hpp>
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
