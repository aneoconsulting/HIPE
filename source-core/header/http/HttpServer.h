//@HIPE_LICENSE@
#pragma once

#pragma warning(push, 0)
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#pragma warning(pop)

#include <http/Server.h>

#include <unordered_map>
#include <thread>
#include <functional>
#include <iostream>
#include <sstream>
#include <core/Logger.h>

namespace http {
	typedef boost::asio::ip::tcp::socket HTTP;


	template<>
	class Server<HTTP> : public ServerBase<HTTP>{

	public:
		std::thread * server_thread;
		static core::Logger logger;

	public:
		Server(unsigned short port, size_t num_threads = 1, long timeout_request = 5, long timeout_content = 300) :
			ServerBase<HTTP>::ServerBase(port, num_threads, timeout_request, timeout_content)
		{
			logger << "Start Http Server on port " + std::to_string(port);
		}
		

	protected:
		void accept() override; 


		
	};

	typedef Server<HTTP> HttpServer;
	

	int start_http_server(int port, http::HttpServer & server, std::thread & thread);
}

