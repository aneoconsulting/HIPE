#pragma once

//%HIPE_LICENSE%

#include <corefilter/tools/filterMacros.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <coredata/Data.h>
#include <corefilter/IFilter.h>
#include <thread>

#include <core/Logger.h>
#include "corefilter/tools/net/WebsocketServer.h"

#ifdef LINUX
#include <sys/utsname.h>
#endif
namespace net
{
	namespace log
	{
		class HipeGlogWebSink : public google::LogSink
		{
			WebsocketServer* _websocket;
			std::shared_ptr<void> _clientConnector;
			std::mutex guard;
		public:
			void regiterSink()
			{
				std::lock_guard<std::mutex> lock(guard);
				google::AddLogSink(this);
			}
			void unregiterSink()
			{
				std::lock_guard<std::mutex> lock(guard);
				google::RemoveLogSink(this);
			}

			HipeGlogWebSink(WebsocketServer* websocket, std::shared_ptr<void> clientConnector) :
				_websocket(websocket),
				_clientConnector(clientConnector)
			{
			}

			static void GetHostName(std::string* hostname)
			{
#if defined(LINUX)
				struct utsname buf;
				if (0 != uname(&buf)) {
					// ensure null termination on failure
					*buf.nodename = '\0';
				}
				*hostname = buf.nodename;
#elif defined(WIN32)
				char buf[MAX_COMPUTERNAME_LENGTH + 1];
				DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
				if (GetComputerNameA(buf, &len))
				{
					*hostname = buf;
				}
				else
				{
					hostname->clear();
				}
#else
#warning There is no way to retrieve the host name.
					*hostname = "(unknown)";
#endif
			}

			char msg[4096];


			std::string formatMessage(const char* message, size_t message_len)
			{
				message_len = (message_len > 4095) ? 4095 : message_len;

				std::string stdMessage;
				strncpy(msg, message, message_len);
				msg[message_len] = 0;
				stdMessage = msg;

				return stdMessage;
			}

			void send(google::LogSeverity severity, const char* full_filename,
			          const char* base_filename, int line,
			          const struct ::tm* tm_time,
			          const char* message, size_t message_len)
			{
				static std::atomic<bool> recursive(false);
				std::lock_guard<std::mutex> lock(guard);
				if (!recursive.exchange(true))
				{
					
					std::stringstream build;
					std::string hostname;
					GetHostName(&hostname);

					char code = 'I';

					switch (severity)
					{
					case 0:
						code = 'I';
						break;
					case 1:
						code = 'W';
						break;
					case 2:
						code = 'E';
						break;
					case 3:
						code = 'F';
						break;
					}

					build << code << ":" << hostname << ":" << base_filename << ":" << line << ":" << formatMessage(
						message, message_len);
					std::string message_str = build.str();
					//websocket.stringStack.push(build.str()); ???
					
					try
					{
						_websocket->send(_clientConnector, message_str, websocketpp::frame::opcode::TEXT);
					}
					catch (std::exception& e)
					{
						std::cerr << "Cannot send message to the web client" << std::endl;
					}

					recursive.exchange(false);
				}
			}
		};

		class FILTER_EXPORT ForwardLogToWeb : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR(data::Data, data::Data);

			SET_NAMESPACE("Net/Log")

			REGISTER(ForwardLogToWeb, ()), _connexData(data::INOUT)
			{
				isActive = false;
				port = 9136;
			}

			void registerClient(std::shared_ptr<void> i_clientConnector);

			void unRegisterClient(std::shared_ptr<void> i_clientConnector);

			REGISTER_P(int, port);

			std::mutex guard;

			std::atomic<bool> isActive;
			std::shared_ptr<std::thread> server;
			WebsocketServer* websocket;
			
			std::map<std::shared_ptr<void>, HipeGlogWebSink* > tcpSink;

			void initServerConnection();

			HipeStatus process();

			void unRegisterAllClient();
			virtual void dispose();

			virtual void onLoad(void* data);
		};

		ADD_CLASS(ForwardLogToWeb, port);
	}
}
