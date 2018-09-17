
#pragma warning(push, 0)
#include <boost/asio/ip/tcp.hpp>
#include <boost/iostreams/stream.hpp>
#pragma warning(pop)
#include <regex>
#include <mutex>

#include <algos/streaming/PopGraphToNode.h>
#include <data/FileVideoInput.h>

#include <data/ImageData.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>
#include <boost/asio/connect.hpp>


#define TIMEOUT std::chrono::seconds(5)
#define MAX_MESSAGE_SIZE 4096
using boost::asio::ip::tcp;

std::shared_ptr<boost::asio::deadline_timer> filter::algos::PopGraphToNode::get_timeout_timer(int nb_seconds) {

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


void filter::algos::PopGraphToNode::Connect()
{
	connector = std::make_shared<HipeConnection>(service);

	//Split address if port is in address
	{
		std::string delimiter = ":";

		size_t pos = 0;
		std::vector<std::string> token;
		std::string target_adress = address;

		while ((pos = target_adress.find(delimiter)) != std::string::npos) {
			token.push_back(target_adress.substr(0, pos));

			target_adress.erase(0, pos + delimiter.length());
		}
		//Push the last argument if pos != size
		if (target_adress != "")
		{
			token.push_back(target_adress);
		}

		if (token.size() > 2)
			throw HipeException("Invalid adress" + address);
		if (token.size() == 2)
		{
			address = token[0];
			std::string::size_type sz;   // alias of size_t

			port = std::stoi(token[1], &sz);

		}
		std::cout << "Slave Adress to request task : " << address << ":" << port << std::endl;
	}


	if (connector && (!connector->socket() || !connector->socket()->is_open()))
	{
		std::unique_ptr<boost::asio::ip::tcp::resolver::query> query;
		/*std::stringstream build_port;
		build_port << port;*/
		query = ::make_unique<boost::asio::ip::tcp::resolver::query>(address, std::to_string(port));


		boost::asio::ip::tcp::resolver resolver(*service);
		std::cout << "Connecting to slave " << address << ":" << std::to_string(port) << std::endl;

		resolver.async_resolve(*query, [this](const boost::system::error_code &ec,
			boost::asio::ip::tcp::resolver::iterator it) {
			if (!ec) {
				{
					std::lock_guard<std::mutex> lock(socket_mutex);
					if (connector && !connector->socket())
						connector->socket() = std::make_shared<TCP>(*service);
				}

				auto timer = get_timeout_timer(15);
				boost::asio::async_connect(*connector->socket(), it, [this, timer]
				(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator /*it*/) {
					std::cout << "Connected to master " << address << std::to_string(port) << std::endl;
					if (timer)
						timer->cancel();
					if (!ec) {
						boost::asio::ip::tcp::no_delay option(true);
						if (connector) connector->socket()->set_option(option);

						while (a_isActive)
						{

							cv::Mat result;
							try
							{
								if (connector) this->connector->read(result);
								data::ImageData image(result);
								this->imagesStack.push(image);
							}
							catch (const HipeException& err)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);
								if (connector)
								{
									this->connector->socket()->close();
								}
								a_isActive.exchange(false);
								service->stop();
								//throw err;
							}
							catch (...)
							{
								std::lock_guard<std::mutex> lock(socket_mutex);
								if (connector)
								{
									this->connector->socket()->close();
								}
								a_isActive.exchange(false);
								service->stop();
								//throw HipeException("Unkown error during SerialNetDataSource readeing");

							}

						}
						std::lock_guard<std::mutex> lock(socket_mutex);
						if (connector) this->connector->socket()->close();
						a_isActive.exchange(false);
						service->stop();

					}
					else {
						std::lock_guard<std::mutex> lock(socket_mutex);
						a_isActive.exchange(false);
						service->stop();
						//throw boost::system::system_error(ec);
					}
				});
			}
			else {
				std::lock_guard<std::mutex> lock(socket_mutex);
				if (connector && connector->socket()) connector->socket()->close();
				throw boost::system::system_error(ec);
			}
		});
		service->reset();
		service->run();
	}

}

HipeStatus filter::algos::PopGraphToNode::process()
{


	if (!a_isActive.exchange(true))
	{
		service = std::make_shared<boost::asio::io_service>();

		thr_client = ::make_unique<boost::thread>([this]() { 
		Connect();

		bool finish = true;

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
		stop();
		return END_OF_STREAM;
	}

	this->imagesStack.clear();
	PUSH_DATA(image);


	return OK;
}


void filter::algos::PopGraphToNode::stop()
{
	//connector.Close();
	int retry = 10;
	a_isActive = false;
	imagesStack.clear();
	//boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
	while (service && !service->stopped() && retry >= 0)
	{
		//boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		service->stop();
		if (connector && connector->socket())
			connector->socket()->close();

		if (thr_client && thr_client->joinable())
		{
			thr_client->join();
		}
		retry--;
	}

	if (service && !service->stopped())
	{
		throw HipeException("Fail to stop SerialData server");
	}
}


void filter::algos::PopGraphToNode::dispose()
{
	stop();
}
