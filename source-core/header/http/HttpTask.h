//@HIPE_LICENSE@
#pragma once
#include <memory>
#include "Response.h"
#include "HttpServer.h"
namespace http {
	class HttpTask
	{
		std::shared_ptr<RawResponse> & _response;

		std::shared_ptr<RawRequest> & _request;

	public:

		HttpTask(std::shared_ptr<RawResponse> & response, std::shared_ptr<http::RawRequest> & request) :
			_response(response), _request(request)
		{};

		static void readFileContent(const std::string& local_path, std::stringstream& data_response);
		void RenderHtml() const;
		void runTask() const;

	};
}
