//@HIPE_LICENSE@

#include <regex>
#include <algos/streaming/SerialNetSlaveSender.h>
#include <data/FileVideoInput.h>
#include <http/HttpClient.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>
#pragma warning(push, 0)


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


		sender = SerialNetDataServer(target_port, 1); // Warning need to verify the port to use when multiple slave are requested
	
		SerialNetDataServer::ptr_func func = &SerialNetDataServer::ImagesHandler;
		sender.startServer(target_port, std::bind(func, &sender));



}


HipeStatus filter::algos::SerialNetSlaveSender::process()
{
	while (_connexData.size() != 0)
	{
		data::ImageData value = _connexData.pop();
		//Initialisation Create Json graph from here to the next PopGraph node
		if (value.This_const().getType() != data::IMGF)
			continue;

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