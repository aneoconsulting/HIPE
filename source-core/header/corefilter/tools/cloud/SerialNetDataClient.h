//%HIPE_LICENSE%
#pragma once
#pragma warning(push, 0) 
#include <opencv2/core/mat.hpp>
#include <atomic>
#include <boost/asio.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#pragma warning(pop) 

#include <core/queue/ConcurrentQueue.h>


#include <corefilter/tools/cloud/SerialNetDataServer.h>
#include <mutex>
#include <coredata/Data.h>

class FILTER_EXPORT SerialNetDataClient
{
	int _bufferingSize;
	std::shared_ptr<HipeConnection> connector;
	std::shared_ptr<boost::asio::io_service> ioservice;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
	std::mutex socket_mutex;
	
	std::unique_ptr<boost::thread> thr_server;
	std::shared_ptr<boost::thread> thr_client;
	std::atomic<bool> a_isActive;

	typedef boost::asio::ip::tcp::socket TCP;
	

	
public:
	typedef void(SerialNetDataClient::* ptr_func)();

	core::queue::ConcurrentQueue<cv::Mat> imagesStack;
	core::queue::ConcurrentQueue<std::string> stringStack;

	std::atomic<bool> & isActive()
	{
		return a_isActive;
	}

	

	void ImageHandler();
	
	void TextHandler();
	void TextSenderHandler();

#ifdef WIN32
	void StartOnceAndConnect(const std::string& address, int port, std::_Binder<std::_Unforced, void( SerialNetDataClient::*&)(), SerialNetDataClient*> binder);
#else
	void StartOnceAndConnect(const std::string& address, int port, std::function<void()> binder);
#endif


	SerialNetDataClient() : _bufferingSize(1)
	{
		a_isActive = false;
	}

	SerialNetDataClient(int port, int bufferingSize = 1) : _bufferingSize(bufferingSize)
	{
		a_isActive = false;
	}

	SerialNetDataClient(const SerialNetDataClient & right) : _bufferingSize(right._bufferingSize), connector(right.connector)
	{
		a_isActive = right.a_isActive.load();
	}

	void operator=(const SerialNetDataClient & right) 
	{
		_bufferingSize = (right._bufferingSize);
		connector = (right.connector);

		a_isActive = right.a_isActive.load();
		ioservice = right.ioservice;
	}

	#ifdef WIN32
	void Connect(const std::string address, int port, std::_Binder<std::_Unforced, void(SerialNetDataClient::*&)(), SerialNetDataClient*> binder);
	#else
	void Connect(const std::string address, int port, std::function<void()> binder);
	#endif
	std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(int nb_seconds);

	void send(const cv::Mat image) const;
	void send(const std::string text) const;
	void read(cv::Mat& image) const;
	void read(std::string & text) const;

	//inline void run() { if (connector) connector->run();  }

	void stop();
};

