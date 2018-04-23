//@HIPE_LICENSE@
#pragma once

#pragma warning(push, 0)
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/streambuf.hpp>
#pragma warning(pop)


namespace http
{
	template <class socket_type>
	class Response : public std::ostream
	{
		
	public:
		boost::asio::streambuf streambuf;

		std::shared_ptr<socket_type> socket;

		
		Response(const std::shared_ptr<socket_type>& socket) : std::ostream(&streambuf), socket(socket)
		{
		}

	public:
		size_t size()
		{
			return streambuf.size();
		}
	};
}