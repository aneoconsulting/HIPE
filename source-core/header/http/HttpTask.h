//@HIPE_LICENSE@
#pragma once
#include <memory>
#include "Response.h"
#include "HttpServer.h"
#include "ProcessController.h"

namespace http {
	class HttpTask
	{
		std::shared_ptr<RawResponse> & _response;

		std::shared_ptr<RawRequest> & _request;

		ProcessController * process;

	public:

		HttpTask(std::shared_ptr<RawResponse> & response, std::shared_ptr<http::RawRequest> & request) :
			_response(response), _request(request), process(nullptr)
		{}

		~HttpTask()
		{
		}

		void run_process();

		static void readFileContent(const std::string& local_path, std::stringstream& data_response);
		void RenderHtml() const;
		void runTask() const;

	};
}
