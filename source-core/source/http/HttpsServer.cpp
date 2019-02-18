#include <HttpsServer.h>
#include "HttpTask.h"

namespace http
{
	int start_https_server(int port, http::HttpsServer & server, std::thread & thread)
	{
		server.config.num_threads = 1;
		server.config.port = port;
		LOG(INFO) << "Waiting for Json request ...";
		server.resource["^/json$"]["POST"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {

			http::HttpTask task(response, request);
#ifdef UNIX // For now running on thread only
			task.runTask();
#else
			task.run_process();
#endif
		};

		server.resource["^/$"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/[A-Za-z]*.html$"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/[A-Za-z]*.ico$"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/demos/[A-Za-z]*.json$"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/css.*"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/js/.*.js"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};
		server.resource["^/images/.*"]["GET"] = [](std::shared_ptr<http::RawResponse> response, std::shared_ptr<http::RawRequest> request) {
			http::HttpTask task(response, request);
			task.RenderHtml();
		};

		thread = server.run();

		return port;
	}
}
