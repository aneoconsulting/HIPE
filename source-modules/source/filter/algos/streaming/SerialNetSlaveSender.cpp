#include <algos/streaming/SerialNetSlaveSender.h>
#include <data/FileVideoInput.h>
#include <http/HttpClient.h>
#include <regex>
#include <stack>

#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>
#include "algos/streaming/SerialNetDataSender.h"
#include <boost/asio/ip/address.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <chrono>
#include <boost/asio/connect.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read.hpp>
#pragma warning(pop)


void filter::algos::SerialNetSlaveSender::startSerialNetServer()
{
		//Cut port in host_or_ip if exist
		{
			std::string delimiter = ":";

			size_t pos = 0;
			std::vector<std::string> token;
			std::string target_adress = host_or_ip;

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
				throw HipeException("Invalid adress" + host_or_ip);
			if (token.size() == 2)
			{
				host_or_ip = token[0];
				std::string::size_type sz;   // alias of size_t

				target_port = std::stoi(token[1], &sz);

			}
			std::cout << "Master Adress to send result : " << host_or_ip << ":" << target_port << std::endl;
		}


		sender = SerialNetDataSender(target_port, 1); // Warning need to verify the port to use when multiple slave are requested
		sender.startServer(target_port);



}


HipeStatus filter::algos::SerialNetSlaveSender::process()
{
	while (_connexData.size() != 0)
	{
		data::ImageData value = _connexData.pop();
		//Initialisation Create Json graph from here to the next PopGraph node
	

		if (sender.isActive())
		//Serialize data and send it
			sender.imagesStack.push(value.getMat());
		else
		{
			return END_OF_STREAM;
		}
	}

	return OK;
}

/**
* \brief Be sure to call the dispose method before to destroy the object SerialNetSlaveSender
*/
void  filter::algos::SerialNetSlaveSender::dispose()
{
	a_isActive = false;
	sender.isActive() = false;
	sender.stop();
	
}


void filter::algos::SerialNetSlaveSender::onStart(void* data)
{
	if (!a_isActive.exchange(true))
	{
		startSerialNetServer();
	} // end of init

};