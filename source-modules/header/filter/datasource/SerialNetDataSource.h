//@HIPE_LICENSE@
#pragma once
#include <coredata/NoneData.h>
#include <coredata/IODataType.h>

#include <coredata/ConnexData.h>
#include <corefilter/tools/filterMacros.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeStatus.h>
#include <corefilter/Model.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>

#include <string>
#include <data/FileImageData.h>
#include <data/VideoData.h>
#include <corefilter/datasource/DataSource.h>
#include "algos/streaming/SerialNetDataSender.h"
#include <mutex>


struct ClientHipeService;

namespace filter
{

	namespace datasource
	{
		class FILTER_EXPORT SerialNetDataSource : public IFilter, public DataSource
		{
			typedef boost::asio::ip::tcp::socket TCP;

			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

			REGISTER(SerialNetDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::VIDF;
			
				a_isActive = false;
			}

			REGISTER_P(int, port);
			REGISTER_P(std::string, address);

			REGISTER_P(std::string, sourceType);

			data::IODataType eSourceType;

			std::shared_ptr<HipeConnection> connector;
			std::shared_ptr<boost::asio::io_service> service;
		public:
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;
			std::mutex socket_mutex;

			SerialNetDataSource(const SerialNetDataSource &left)
			{
				this->a_isActive.exchange(left.a_isActive);
				
				this->port = left.port;
				this->sourceType = left.sourceType;
				this->eSourceType = left.eSourceType;
			}

			virtual data::IODataType getSourceType() const
			{
				return eSourceType;
			}

			void setSourceType(data::IODataType source_type)
			{
				eSourceType = source_type;
			}
	
			std::atomic<bool> a_isActive;

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
			
			HipeStatus process();

			/**
			* \brief Be sure to call the dispose method before to destroy the object SerialNetDataSource
			*/
			virtual void dispose();

			HipeStatus intialize() override;
		};

		ADD_CLASS(SerialNetDataSource, address, port, sourceType);
	}
}
