//%HIPE_LICENSE%


#include <corefilter/tools/net/ForwardNetLog.h>
#include "core/Logger.h"

void net::log::ForwardNetLog::initClientConnection()
{
	if (!isActive.exchange(true))
	{
		SerialNetDataClient::ptr_func func = &SerialNetDataClient::TextSenderHandler;

		serialNetDataClient.StartOnceAndConnect(address, port, std::bind(func, &serialNetDataClient));
		if (!tcpSink) tcpSink = ::make_unique<HipeGlogTCPSink>(serialNetDataClient);
		HipeGlogTCPSink* hipe_glog_tcp_sink = tcpSink.get();

		google::AddLogSink(hipe_glog_tcp_sink);

	}
}

HipeStatus net::log::ForwardNetLog::process()
{
	if (_connexData.empty()) return OK;

	initClientConnection();
	while (!_connexData.empty())
	{
		_connexData.pop();
	}

	return OK;
}

void net::log::ForwardNetLog::dispose()
{
	std::string message = "Disconnect Log Forwarder";
	std::string full_filename = __FILE__;
	if (tcpSink) 
		tcpSink->send(google::GLOG_INFO, __FILE__, extractFileName(full_filename).c_str(), __LINE__, nullptr, message.c_str(), message.size());

	if (isActive)
	{
		if (tcpSink) {
			google::RemoveLogSink(tcpSink.get());
		}
		isActive.exchange(false);
		serialNetDataClient.stop();
	}
}