//%HIPE_LICENSE%

#include <tools/filterMacros.h>
#include <tools/RegisterTools.hpp>
#include <tools/RegisterTable.h>
#include <coredata/Data.h>
#include <corefilter/IFilter.h>
#include <tools/cloud/SerialNetDataServer.h>
#include <thread>

namespace net
{
	namespace log
	{
		class FILTER_EXPORT AttachNetLog : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR(data::Data, data::Data);

			SET_NAMESPACE("Net/Log")


			REGISTER(AttachNetLog, ()), _connexData(data::INOUT)
			{
				isActive = false;
				port = 9000;
			}

			std::atomic<bool> isActive;
			SerialNetDataServer serialNetDataServer;
			std::shared_ptr<std::thread> server;

			REGISTER_P(int, port);

			void initServer();
			HipeStatus process();

			virtual void dispose();
		};

		ADD_CLASS(AttachNetLog, port);
	}
}
