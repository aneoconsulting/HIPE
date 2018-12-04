//@HIPE_LICENSE@
#pragma once
#include <istream>
#include "Server.h"

namespace http
{

	class Content : public std::istream
	{
		
	public:
		size_t size()
		{
			return streambuf.size();
		}

		std::string string()
		{
			std::stringstream ss;
			ss << rdbuf();
			return ss.str();
		}

	public:
		boost::asio::streambuf& streambuf;

		Content(boost::asio::streambuf& streambuf) : std::istream(&streambuf), streambuf(streambuf)
		{
		}
	};
}
