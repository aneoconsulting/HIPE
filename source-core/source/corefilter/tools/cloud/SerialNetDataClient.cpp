//HIPE_LICENSE

#include <iostream>
#include <thread>
#include <mutex>

#include <corefilter/tools/cloud/SerialNetDataClient.h>
#include <core/HipeException.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>


using namespace boost::asio;
using boost::asio::ip::tcp;

typedef boost::iostreams::stream < boost::iostreams::back_insert_device<std::string> > archivestream;

typedef boost::shared_ptr<Connection> connection_ptr;

void SerialNetDataClient::ImageHandler()
{
	cv::Mat result;
	this->connector->read(result);
	
	this->imagesStack.push(result);
}

void SerialNetDataClient::TextHandler()
{
	std::string result;

	this->connector->read(result);
	
	this->stringStack.push(result);
}

void SerialNetDataClient::TextSenderHandler()
{
	std::string text;
	if (!this->stringStack.trypop_until(text, 3000))
		return;

	this->connector->send(text);
	this->stringStack.clear();
}

typedef void(SerialNetDataClient::* ptr_func)();

#ifdef WIN32
void SerialNetDataClient::StartOnceAndConnect(const std::string& address, int port, std::_Binder<std::_Unforced, void(SerialNetDataClient::*&)(), SerialNetDataClient*> binder)
#else
	void SerialNetDataClient::StartOnceAndConnect(const std::string& address, int port, std::function<void()> binder)
#endif
{
	if (!a_isActive.exchange(true))
	{
		thr_client = std::make_shared<boost::thread>([this, address, port, binder]() 
		{
			
			Connect(address, port, binder);
		});

	}
}

std::shared_ptr<boost::asio::deadline_timer> SerialNetDataClient::get_timeout_timer(int nb_seconds) {

	auto timer = std::make_shared<boost::asio::deadline_timer>(*ioservice);
	timer->expires_from_now(boost::posix_time::seconds(nb_seconds));
	timer->async_wait([this](const boost::system::error_code& ec) {
		if (!ec) {
			std::lock_guard<std::mutex> lock(socket_mutex);
			if (connector && connector->socket()) {
				boost::system::error_code err;
				connector->socket()->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
				connector->socket()->lowest_layer().close();
			}
		}
	});
	return timer;
}

#ifdef WIN32
void SerialNetDataClient::Connect(const std::string address, int port, std::_Binder<std::_Unforced, void(SerialNetDataClient::*&)(), SerialNetDataClient*> binder)
#else
	void SerialNetDataClient::Connect(const std::string address, int port, std::function<void()> binder)
#endif
{
	if (!ioservice) ioservice = std::make_shared<boost::asio::io_service>();

	if (!connector) connector = std::make_shared<HipeConnection>(ioservice);

	if (connector && (!connector->socket() || !connector->socket()->is_open()))
	{
		std::unique_ptr<boost::asio::ip::tcp::resolver::query> query;
		std::stringstream build_port;
		build_port << port;
		query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(new boost::asio::ip::tcp::resolver::query(address, std::to_string(port)));


		boost::asio::ip::tcp::resolver resolver(*ioservice);
		std::cout << "Connecting to master " << address << std::to_string(port) << std::endl;


		resolver.async_resolve(*query, [this, address, port, binder](const boost::system::error_code &ec,
			boost::asio::ip::tcp::resolver::iterator it) 
		{
		                       
			if (!ec) {
				{
					std::lock_guard<std::mutex> lock(socket_mutex);
					connector->socket() = std::make_shared<TCP>(*ioservice);
				}
				

				auto timer = get_timeout_timer(15);
				boost::asio::async_connect(*this->connector->socket(), it, [this, timer, address, port, binder]
				(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator /*it*/) {
					std::cout << "Connected to master " << address << std::to_string(port) << std::endl;
					if (timer)
						timer->cancel();
					if (!ec) {
						boost::asio::ip::tcp::no_delay option(true);
						this->connector->socket()->set_option(option);

						while (a_isActive)
						{
							
							try
							{
								binder();
							}
							catch (const HipeException& err)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);
								this->connector->socket()->close();
								a_isActive.exchange(false);
								ioservice->stop();
								//throw err;
							}
							catch (...)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);

								a_isActive.exchange(false);
								ioservice->stop();
								//throw HipeException("Unkown error during SerialNetDataSource reading");

							}

						}

					}
					else {
						std::lock_guard<std::mutex> lock(socket_mutex);
						a_isActive.exchange(false);
						ioservice->stop();

						std::cerr << "Fail to connect to the server reason : " << ec.message() << std::endl;
						//throw boost::system::system_error(ec);
					}
				});
			}
			else {
				std::lock_guard<std::mutex> lock(socket_mutex);
				throw boost::system::system_error(ec);
			}
		});


		ioservice->reset();
		ioservice->run();
	}

}

void SerialNetDataClient::send(const cv::Mat image) const
{
	if (connector) connector->send(image);
}

void SerialNetDataClient::send(const std::string text) const
{
	if (connector) connector->send(text);
}


void SerialNetDataClient::read(cv::Mat & image) const
{
	if (connector) connector->read(image);
}

void SerialNetDataClient::read(std::string& text) const
{
	if (connector) connector->read(text);
}

void SerialNetDataClient::stop()
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

