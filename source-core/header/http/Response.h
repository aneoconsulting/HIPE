//@HIPE_LICENSE@
#pragma once

#pragma warning(push, 0)
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/streambuf.hpp>
#pragma warning(pop)


namespace http
{
	class RawResponse : public std::ostream
	{
	public:
		boost::asio::streambuf streambuf;

		RawResponse() : std::ostream(&streambuf)
		{
			
		}

		size_t size() const
		{
			return streambuf.size();
		}

	};

	template <class socket_type>
	class Response : public RawResponse
	{
	public:
		std::shared_ptr<socket_type> socket;

	
		Response(const std::shared_ptr<socket_type>& socket) : socket(socket)
		{
			
		}
	};


}