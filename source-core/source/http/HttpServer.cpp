//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <HttpServer.h>
#include <HttpTask.h>

namespace http
{
	core::Logger HttpServer::logger = core::setClassNameAttribute("HttpServer");

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
		server.logger << "Waiting for Json request ...";
		server.resource["^/json$"]["POST"] = [](std::shared_ptr<Response<http::HTTP>> response, std::shared_ptr<http::Request<http::HTTP>> request) {
			http::HttpTask task(response, request);
			/*	thread work_thread([&response, &request] {


			});
			work_thread.detach();
			*/
			

			task.runTask();
		};
		thread = server.run();

		//server_thread.detach();

		return port;
	}
}
