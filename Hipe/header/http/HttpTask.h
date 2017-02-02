#pragma once
#include <memory>
#include "Response.h"
#include "HttpServer.h"

namespace http {
	class HttpTask
	{
		std::shared_ptr<Response<http::HTTP>> & _response;

		std::shared_ptr<Request<http::HTTP>> & _request;

	public:
		static core::Logger logger;

		HttpTask(std::shared_ptr<Response<http::HTTP>> & response, std::shared_ptr<http::Request<http::HTTP>> & request) :
			_response(response), _request(request)
		{};

		void runTask();

	};
}
