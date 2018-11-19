#include <corefilter/tools/net/ForwardLogToWeb.h>
#include "tools/Localenv.h"
#include <mutex>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/logger/levels.hpp>


void net::log::ForwardLogToWeb::registerClient(std::shared_ptr<void> i_clientConnector)
{
	std::lock_guard<std::mutex> lock(guard);

	if (tcpSink.find(i_clientConnector) == tcpSink.end())
	{
		HipeGlogWebSink* sink = new HipeGlogWebSink(websocket, i_clientConnector);
		sink->regiterSink();
		tcpSink[i_clientConnector] = sink;
	}
}

void net::log::ForwardLogToWeb::unRegisterClient(std::shared_ptr<void> i_clientConnector)
{
	std::lock_guard<std::mutex> lock(guard);

	if (tcpSink.find(i_clientConnector) != tcpSink.end())
	{
		HipeGlogWebSink* sink = tcpSink[i_clientConnector];
		if (sink == nullptr)
		{
			tcpSink.erase(i_clientConnector);
			return;
		}

		sink->unregiterSink();
		tcpSink.erase(i_clientConnector);
	}
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
		
	this->unRegisterClient(hdl.lock());

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

	return OK;
}

void net::log::ForwardLogToWeb::unRegisterAllClient()
{
	std::lock_guard<std::mutex> lock(guard);
	tcpSink.clear();
}

void net::log::ForwardLogToWeb::dispose()
{
	unRegisterAllClient();

	if (websocket != nullptr && !websocket->stopped()) {
			
			websocket->stop();
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
		unRegisterAllClient();
		std::error_code er;
		websocket->stop_listening(er);
		
	});

}
