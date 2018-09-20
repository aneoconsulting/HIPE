//@HIPE_LICENSE@
#pragma once
#include <string>
#pragma warning(push, 0)
#include <glog/logging.h>
#include <iostream>
#pragma warning(pop)

#define LOG_CONCAT2(a, b) a b

#ifdef WIN32
#define LOG_DIR "c:\\temp\\"
#else
#define LOG_DIR "/tmp/"
#endif

#define LOG_PATH() LOG_CONCAT2(LOG_DIR, "Hipe.log")
#define LOG_PATH_ROUND() LOG_CONCAT2(LOG_DIR, "Hipe_3%N.log")


DECLARE_string(log_backtrace_at);  // logging.cc


#define PRINTSTACK(severity) \
{ \
	char stack_print[100]; \
	snprintf(stack_print, 100, "%s:%d", removeDirectoryName(__FILE__).c_str(), __LINE__); \
	FLAGS_log_backtrace_at = stack_print; \
	LOG(severity) << "***** Stacktrace end ******"; \
}\

namespace core
{
	class HipeGlogSink : public google::LogSink
	{
		void send(google::LogSeverity severity, const char* full_filename,
			const char* base_filename, int line,
			const struct ::tm* tm_time,
			const char* message, size_t message_len)
		{
			std::cout << "Test TCP GLOG SYNC" << base_filename << ":" << line << " " << message;
		}
	};

	class Logger
	{
	public:

		Logger() 
		{
			
		}

		//Logger() {}
		

		template <typename T>
		Logger& operator<<(const T& value)
		{
			LOG(INFO) << value;
			

			return *this;
		}

		Logger& operator<<(int iLevel)
		{

			return *this;
		}

		static void init(const char *filename);
		
	};
	
	

}
