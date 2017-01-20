#pragma once
#include "Server.h"

namespace http
{
	class Config
	{
	public:
		Config(unsigned short port, size_t num_threads) : num_threads(num_threads), port(port), reuse_address(true)
		{
		}
	
		size_t num_threads;
	
		unsigned short port;
		///IPv4 address in dotted decimal form or IPv6 address in hexadecimal notation.
		///If empty, the address will be any address.
		std::string address;
		///Set to false to avoid binding the socket to an address that is already in use.
		bool reuse_address;
	};
}