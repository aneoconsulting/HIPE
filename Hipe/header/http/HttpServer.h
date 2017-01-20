#ifndef SERVER_HTTP_HPP
#define	SERVER_HTTP_HPP

#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>

#include <http/Server.h>

#include <unordered_map>
#include <thread>
#include <functional>
#include <iostream>
#include <sstream>

namespace http {
	typedef boost::asio::ip::tcp::socket HTTP;


	template<>
	class Server<HTTP> : public ServerBase<HTTP>{

	public:
		std::thread * server_thread;

	public:
		Server(unsigned short port, size_t num_threads = 1, long timeout_request = 5, long timeout_content = 300) :
			ServerBase<HTTP>::ServerBase(port, num_threads, timeout_request, timeout_content) {}
		

	protected:
		void accept() override; 


		
	};

	typedef Server<HTTP> HttpServer;


	int start_http_server(int port, http::HttpServer & server, std::thread & thread);
}
#endif	/* SERVER_HTTP_HPP */
