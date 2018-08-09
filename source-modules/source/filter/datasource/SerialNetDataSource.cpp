//@HIPE_LICENSE@
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <filter/datasource/SerialNetDataSource.h>
#include <data/FileVideoInput.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#include "algos/streaming/SerialNetDataSender.h"
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <mutex>

#define TIMEOUT std::chrono::seconds(5)
#define MAX_MESSAGE_SIZE 4096
using boost::asio::ip::tcp;

enum { max_length = 2048 };

std::shared_ptr<boost::asio::deadline_timer> filter::datasource::SerialNetDataSource::get_timeout_timer(int nb_seconds) {
	
	auto timer = std::make_shared<boost::asio::deadline_timer>(*service);
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


void filter::datasource::SerialNetDataSource::Connect()
{
	connector = std::make_shared<HipeConnection>(service);

	if (connector && (!connector->socket() || !connector->socket()->is_open()))
	{
		std::unique_ptr<boost::asio::ip::tcp::resolver::query> query;
		std::stringstream build_port;
		build_port << port;
		query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(new boost::asio::ip::tcp::resolver::query(address, std::to_string(port)));

		
		boost::asio::ip::tcp::resolver resolver(*service);
		std::cout << "Connecting to master " << address << std::to_string(port) << std::endl;

		resolver.async_resolve(*query, [this](const boost::system::error_code &ec,
			boost::asio::ip::tcp::resolver::iterator it) {
			if (!ec) {
				{
					std::lock_guard<std::mutex> lock(socket_mutex);
					connector->socket() = std::make_shared<TCP>(*service);
				}

				auto timer = get_timeout_timer(15);
				boost::asio::async_connect(*this->connector->socket(), it, [this, timer]
				(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator /*it*/) {
					std::cout << "Connected to master " << address << std::to_string(port) << std::endl;
					if (timer)
						timer->cancel();
					if (!ec) {
						boost::asio::ip::tcp::no_delay option(true);
						this->connector->socket()->set_option(option);

						while (a_isActive)
						{
							
							cv::Mat result;
							try
							{
								this->connector->read(result);
								data::ImageData image(result);
								this->imagesStack.push(image);
							}
							catch (const HipeException& err)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);
								this->connector->socket()->close();
								a_isActive.exchange(false);
								service->stop();
								//throw err;
							}
							catch (...)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);
								
								a_isActive.exchange(false);
								service->stop();
								//throw HipeException("Unkown error during SerialNetDataSource readeing");

							}
							
						}

					}
					else {
						std::lock_guard<std::mutex> lock(socket_mutex);
						a_isActive.exchange(false);
						service->stop();
						
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
		service->reset();
		service->run();
	}

}

HipeStatus filter::datasource::SerialNetDataSource::process()
{
	boost::thread * thr_client;

	if (!a_isActive.exchange(true))
	{
		thr_client = new boost::thread([this]() { service = std::make_shared<boost::asio::io_service>();
		Connect();
		
		});
		
	}


	//this->read_request_and_content(socket);
	data::ImageData image;
	while (a_isActive)
	{

		if (this->imagesStack.trypop_until(image, 3000)) //
			break;
	}

	if (!a_isActive)
	{
		this->imagesStack.clear();
		if (service) service->stop();
		return END_OF_STREAM;
	}

	this->imagesStack.clear();
	PUSH_DATA(image);

	return OK;
}

void filter::datasource::SerialNetDataSource::dispose()
{
	if (service)
	{
		
	}
}

HipeStatus filter::datasource::SerialNetDataSource::intialize()
{
	
	return OK;
}



