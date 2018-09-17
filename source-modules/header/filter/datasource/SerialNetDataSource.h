//@HIPE_LICENSE@
#pragma once
#include <mutex>
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


#include <corefilter/datasource/DataSource.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>

#include <corefilter/tools/cloud/SerialNetDataClient.h>

#include <boost/asio/ip/tcp.hpp>


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
			}

			REGISTER_P(int, port);
			REGISTER_P(std::string, address);

			REGISTER_P(std::string, sourceType);

			data::IODataType eSourceType;

			SerialNetDataClient serialNetDataClient;


		public:
			
			SerialNetDataSource(const SerialNetDataSource &left)
			{
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
