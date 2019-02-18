//%HIPE_LICENSE%

#include <corefilter/tools/net/AttachNetLog.h>
#include <thread>
#include <glog/logging.h>
#include <glog/raw_logging.h>

static std::vector<std::string> splitMessage(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void net::log::AttachNetLog::initServer()
{
	if (!isActive.exchange(true))
	{
		SerialNetDataServer::ptr_func func = &SerialNetDataServer::TextReceiverHandler;

		serialNetDataServer.startServer(port, std::bind(func, &serialNetDataServer));

		if (!server)
		{
			server = std::make_shared<std::thread>([this]()
			{
				while (isActive)
				{
					std::string log;

					if (!serialNetDataServer.stringStack.trypop_until(log, 3000))
					{
						continue;
					}
					else
					{
						auto arrayMsg = splitMessage(log, ':');
						char sev_char = arrayMsg[0].at(0);
						google::LogSeverity severity;
						switch(sev_char)
						{
						case 'I':
							severity = google::GLOG_INFO;
							break;
						case 'W':
							severity = google::GLOG_WARNING;
							break;
						case 'E':
							severity = google::GLOG_ERROR;
							break;
						case 'F':
							severity = google::GLOG_ERROR; // We don't want to crash the server so Error from slave is enough
							break;

						default:
							LOG(WARNING) << "Unable to detect Severity from NetLog";
							severity = google::GLOG_WARNING; // Use default but need to fix if it's happening
						}

						//TODO parse filename and line from string
						std::string::size_type sz;
						std::stringstream message;
						message << arrayMsg[4];
						for (int i = 4; i < arrayMsg.size(); i++)
						{
							message << ":" << arrayMsg[i];
						}
						google::LogMessage(arrayMsg[2].c_str(), std::stoi(arrayMsg[3], &sz), severity).stream() << "[" << arrayMsg[1] << "] " << message.str();
					}
				}
			});
		}
	}
}

HipeStatus net::log::AttachNetLog::process()
{
	initServer();

	while (!_connexData.empty())
	{
		_connexData.pop();
	}

	//Nothing more to do

	return OK;
}

/**
 * \brief 
 * \return 
 */
void net::log::AttachNetLog::dispose()
{
	isActive = false;
	if (server && server->joinable())
	{
		server->join();
	}
	server.reset();

	serialNetDataServer.stop();

}
