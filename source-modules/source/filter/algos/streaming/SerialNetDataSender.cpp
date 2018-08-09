//HIPE_LICENSE
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <algos/streaming/SerialNetDataSender.h>
#include <core/HipeException.h>
#include <core/HipeStatus.h>

using namespace boost::asio;
using boost::asio::ip::tcp;

typedef boost::iostreams::stream < boost::iostreams::back_insert_device<std::string> > archivestream;

typedef boost::shared_ptr<Connection> connection_ptr;



HipeConnection::HipeConnection(std::shared_ptr<boost::asio::io_service> i_service)
{
	
	_impl_connector = std::make_shared<Connection>(i_service);
	
	ioservice = i_service;
	
}


HipeConnection::~HipeConnection()
{
	//if (_impl_connector)
	//{
	//	

	//	void* ptr_conntection = _impl_connector.get();

	//	Connection * ptr = reinterpret_cast<Connection *>(ptr_conntection);
	//	if (ioservice && !ioservice->stopped())
	//	{
	//		ioservice->stop();
	//	}

	//	if (ptr != nullptr && ptr->socket())
	//	{
	//		ptr->socket()->close();
	//	}
	//}
}

HipeStatus HipeConnection::Cancel()
{
	if (!_impl_connector)
		throw HipeException("There is no connection etablished whit the slave hipe");

	_impl_connector->socket()->cancel();

	return OK;
}


HipeStatus HipeConnection::Close()
{
	if (!_impl_connector)
		throw HipeException("There is no connection etablished whit the slave hipe");

	_impl_connector->socket()->close();

	return OK;
}

std::shared_ptr<boost::asio::ip::tcp::socket>& HipeConnection::socket()
{
	if (!_impl_connector)
		throw HipeException("There is no connection etablished whit the slave hipe");

	
	return _impl_connector->socket();
}

void HipeConnection::run()
{
	ioservice->run();
}


void HipeConnection::write_handler(const boost::system::error_code& ec,
	std::size_t bytes_transferred)
{
	if (ec)
	{
		std::cerr << "Error during transfer to the slave : Err code " << ec << " msg : " << ec.message();
	}
	else
		std::cout << "archive send...Size : " << bytes_transferred << " Bytes";
}

void HipeConnection::send(const cv::Mat& mat) const
{
	if (!_impl_connector)
		throw HipeException("There is no connection etablished whit the slave hipe");


	_impl_connector->async_write(mat, HipeConnection::write_handler);
}

HipeStatus HipeConnection::read(cv::Mat& mat) const
{
	if (!_impl_connector)
		throw HipeException("There is no connection etablished whit the slave hipe");


	_impl_connector->read(mat);

	return OK;
}

void SerialNetDataSender::accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	acceptor->async_accept(*socket, [this, socket](const boost::system::error_code& ec)
                       {
	                       //Immediately start accepting a new connection (if io_service hasn't been stopped)
	                       if (ec != boost::asio::error::operation_aborted)
		                       accept(socket);

	                       if (!ec)
	                       {
		                       boost::asio::ip::tcp::no_delay option(true);
		                       socket->set_option(option);

		                       //this->read_request_and_content(socket);
		                       while (a_isActive)
		                       {
			                       data::ImageData image;
			                       if (!this->imagesStack.trypop_until(image, 3000))
				                       continue;

			                       try
			                       {
				                       this->connector->send(image.getMat());
									   this->imagesStack.clear();
			                       }
			                       catch (boost::system::system_error& ex)
			                       {
				                       std::stringstream error;
									   this->imagesStack.clear();
				                       error << "Writing data is failing check inner exception (error_code) = " << ex.code() << " : \n" << ex.what() << std::endl;
				                       std::cerr << error.str();
									   a_isActive.exchange(false);
				                       throw HipeException(error.str());
			                       }
			                       catch (std::exception& ex)
			                       {
				                       std::stringstream error;
									   this->imagesStack.clear();
				                       error << "Writing data is failing check inner exception : \n" << ex.what() << std::endl;
				                       std::cerr << error.str();
									   a_isActive.exchange(false);
				                       throw HipeException(error.str());
			                       }
		                       }
	                       }
	                       /*else if (on_error)
		                       on_error(std::shared_ptr<Request<HTTP>>(new Request<HTTP>(*socket)), ec);*/
                       });
}

void SerialNetDataSender::startServer(int port)
{
	try
	{
		if (!ioservice)
			ioservice = std::make_shared<boost::asio::io_service>();

		auto timer = std::make_shared<boost::asio::deadline_timer>(*ioservice);
		timer->expires_from_now(boost::posix_time::seconds(3));

		if (!a_isActive.exchange(true))
		{
			thr_server = ::make_unique<boost::thread>([this, port, timer]()
			{
				if (ioservice->stopped())
					ioservice->reset();

				if (!acceptor)
					acceptor = ::make_unique<boost::asio::ip::tcp::acceptor>(*ioservice);

				boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
				acceptor->open(endpoint.protocol());
				acceptor->set_option(boost::asio::socket_base::reuse_address(true));
				acceptor->bind(endpoint);
				acceptor->listen();

				if (!connector) connector = std::make_shared<HipeConnection>(ioservice);

				accept(connector->socket());
				/*accept_handler ah(ios, port, 15);*/
				/*ios.await_operation(std::chrono::seconds(15), [&] {
					a_isActive = false;
					throw HipeException("Timeout to wait for client\n");
				});*/
				if (timer)
				{
					timer->cancel();
				}
				ioservice->run();
			});

			timer->wait();
			
			return;
		}
	}
	catch (std::exception& e)
	{
		a_isActive = false;
		throw HipeException(std::string("Uncatch error during server creation. Inner exception :\n") + e.what());
	}


}

void SerialNetDataSender::send(const cv::Mat image) const
{
	
	if (connector) connector->send(image);
}

void SerialNetDataSender::read(cv::Mat & image) const
{

	if (connector) connector->read(image);
}

void SerialNetDataSender::stop()
{
	//connector.Close();
	int retry = 10;
	a_isActive = false;
	imagesStack.clear();
	//boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
	while (ioservice && !ioservice->stopped() && retry >= 0)
	{
		
		ioservice->stop();

		if (connector && connector->socket())
			connector->socket()->close();

		if (thr_server && thr_server->joinable())
		{
			thr_server->join();
		}
		retry--;
	}
	
	if (ioservice && !ioservice->stopped())
	{
		throw HipeException("Fail to stop SerialData server");
	}
}

