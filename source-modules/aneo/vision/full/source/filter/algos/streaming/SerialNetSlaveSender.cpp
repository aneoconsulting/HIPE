//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */


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
