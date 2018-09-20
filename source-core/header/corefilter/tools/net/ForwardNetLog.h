//%HIPE_LICENSE%

#include <tools/filterMacros.h>
#include <tools/RegisterTools.hpp>
#include <tools/RegisterTable.h>
#include <coredata/Data.h>
#include <corefilter/IFilter.h>
#include <thread>
#include <tools/cloud/SerialNetDataClient.h>
#include <core/Logger.h>
#ifdef LINUX
#include <sys/utsname.h>
#endif
namespace net
{
	namespace log
	{
		class HipeGlogTCPSink : public google::LogSink
		{
			SerialNetDataClient & serialNetDataClient;

		public:
			
			HipeGlogTCPSink(SerialNetDataClient & i_serialNetDataClient) : serialNetDataClient(i_serialNetDataClient)
			{
				
			}

			static void GetHostName(std::string* hostname) {
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
				if (GetComputerNameA(buf, &len)) {
					*hostname = buf;
				}
				else {
					hostname->clear();
				}
#else
#warning There is no way to retrieve the host name.
					*hostname = "(unknown)";
#endif
			}

			void send(google::LogSeverity severity, const char* full_filename,
				const char* base_filename, int line,
				const struct ::tm* tm_time,
				const char* message, size_t message_len)
			{
				
				static std::atomic<bool> recursive(false);

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

					//// The logfile's filename will have the date/time & pid in it
					//std::ostringstream time_pid_stream;
					//time_pid_stream.fill('0');
					//time_pid_stream << 1900 + tm_time->tm_year
					//	<< std::setw(2) << 1 + tm_time->tm_mon
					//	<< std::setw(2) << tm_time->tm_mday
					//	<< '-'
					//	<< std::setw(2) << tm_time->tm_hour
					//	<< std::setw(2) << tm_time->tm_min
					//	<< std::setw(2) << tm_time->tm_sec
					//	<< '.'
					//	<< std::this_thread::get_id();

					//const std::string& time_pid_string = time_pid_stream.str();
					build <<  code <<  ":" << hostname << ":" << base_filename << ":" << line << ":" << message;

					serialNetDataClient.stringStack.push(build.str());
					recursive.exchange(false);
				}
			}
		};

		class FILTER_EXPORT ForwardNetLog : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR(data::Data, data::Data);

			SET_NAMESPACE("Net/Log")

			REGISTER(ForwardNetLog, ()), _connexData(data::INOUT)
			{
				isActive = false;
				port = 9000;
				address = "127.0.0.1";
			}

			REGISTER_P(std::string, address);
			REGISTER_P(int, port);

			std::atomic<bool> isActive;
			std::shared_ptr<std::thread> client;
			SerialNetDataClient serialNetDataClient;
			std::unique_ptr<HipeGlogTCPSink> tcpSink;

			void initClientConnection();

			HipeStatus process();

			virtual void dispose();
		};

		ADD_CLASS(ForwardNetLog, address, port);
	}
}
