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

#include <mutex>

#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <corefilter/tools/cloud/SerialNetDataServer.h>

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class PopGraphToNode : public filter::IFilter
		{
			typedef boost::asio::ip::tcp::socket TCP;

			SET_NAMESPACE("vision/Cloud")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(PopGraphToNode, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				address = "192.168.6.43";
				a_isActive = false;
			}


			std::atomic<bool> a_isActive;
			std::shared_ptr<HipeConnection> connector;
			std::shared_ptr<boost::asio::io_service> service;
		public:
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;
			std::mutex socket_mutex;
			std::unique_ptr<boost::thread> thr_client;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);

			REGISTER_P(std::string, address);
			REGISTER_P(int, port);

			inline std::atomic<bool> & isActive()
			{
				return a_isActive;
			}

			void setactive()
			{
				a_isActive.exchange(true);
			}

			std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(int nb_seconds);

			void Connect();

			HipeStatus process() override;
			void stop();


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose();
		};

		ADD_CLASS(PopGraphToNode, _debug, skip_frame, address);


	}
}
