//@HIPE_LICENSE@
#include <HttpServer.h>
#include <HttpTask.h>

namespace http
{
	void Server<HTTP>::accept() {
		//Create new socket for this connection
		//Shared_ptr is used to pass temporary objects to the asynchronous functions
		auto socket = std::make_shared<HTTP>(*io_service);

		acceptor->async_accept(*socket, [this, socket](const boost::system::error_code& ec){
			//Immediately start accepting a new connection (if io_service hasn't been stopped)
			if (ec != boost::asio::error::operation_aborted)
				accept();

			if (!ec) {
				boost::asio::ip::tcp::no_delay option(true);
				socket->set_option(option);

				this->read_request_and_content(socket);
			}
			else if (on_error)
				on_error(std::shared_ptr<Request<HTTP>>(new Request<HTTP>(*socket)), ec);
		});
	}

	int start_http_server(int port, http::HttpServer & server, std::thread & thread)
	{
		server.config.num_threads = 1;
		server.config.port = port;
		LOG(INFO) << "Waiting for Json request ...";
		server.resource["^/json$"]["POST"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			/*	thread work_thread([&response, &request] {


			});
			work_thread.detach();
			*/
			

			task.runTask();
		};

		server.resource["^/$"]["GET"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/[A-Za-z]*\.html$"]["GET"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/css.*"]["GET"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/js/.*\.js"]["GET"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/images/.*"]["GET"] = [](std::shared_ptr<RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};

		thread = server.run();

		return port;
	}

	
}
