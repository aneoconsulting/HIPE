#include <corefilter/tools/net/ForwardLogToWeb.h>
#include "tools/Localenv.h"


void net::log::ForwardLogToWeb::detach_logger()
{
	std::lock_guard<std::mutex> lock(guard);

	if (tcpSink)
	{
		google::RemoveLogSink(tcpSink.get());

		tcpSink.reset(nullptr);
		LOG(INFO) << "Disconnect log forward from web client" << std::endl;
	}
	LOG(INFO) << "Disconnect log forward finished" << std::endl;
}

void net::log::ForwardLogToWeb::attach_logger()
{
	std::lock_guard<std::mutex> lock(guard);
	if (!tcpSink) {
			 
			tcpSink.reset(new HipeGlogWebSink(websocket, this->registeredClient()));
		}
		google::AddLogSink(tcpSink.get());
		LOG(INFO) << "Start to forward log to client" << std::endl;
}

void net::log::ForwardLogToWeb::initServerConnection()
{
	

websocket = ServerInit(
	[&](WebsocketServer*, websocketpp::connection_hdl) { // onConnection
},
	[&](WebsocketServer* socket, websocketpp::connection_hdl hdl) { // onOpen
		this->registerClient(hdl.lock());
		
},
	[&](WebsocketServer* s, websocketpp::connection_hdl hdl) { // onClose
		
		this->detach_logger();

},
	[&](WebsocketServer*, websocketpp::connection_hdl, message_ptr) { // onClose
		LOG(WARNING) << "Receive unexpected message from client";
},
	corefilter::getLocalEnv().getValue("base_cert"));

	websocket->set_access_channels(websocketpp::log::alevel::none);
}

HipeStatus net::log::ForwardLogToWeb::process()
{
	while (!_connexData.empty())
	{
		_connexData.pop();
	}
	if (!tcpSink && clientConnector) this->attach_logger();
	

	return OK;
}

void net::log::ForwardLogToWeb::dispose()
{
	detach_logger();

	if (websocket != nullptr && !websocket->stopped()) {
			
			websocket->stop();
			clientConnector.reset();
	}

	if (server && server->joinable()) {
		server->join();
		server.reset();
	}

	if (websocket != nullptr) websocket->reset();
}

void net::log::ForwardLogToWeb::onLoad(void* data)
{
	initServerConnection();

	server = std::make_shared<std::thread>([this]()
	{
		websocket->listen(port);

		// Start the server accept loop
		websocket->start_accept();
		websocket->run();
		this->detach_logger();
		std::error_code er;
		websocket->stop_listening(er);
		
	});

}
