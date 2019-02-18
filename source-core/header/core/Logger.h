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

#pragma once
#include <string>
#pragma warning(push, 0)
#include <glog/logging.h>
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags_declare.h>
#pragma warning(pop)

#define LOG_CONCAT2(a, b) a b

#ifdef WIN32
#define LOG_DIR "c:\\temp\\"
#else
#define LOG_DIR "/tmp/"
#endif

#define LOG_PATH() LOG_CONCAT2(LOG_DIR, "Hipe.log")
#define LOG_PATH_ROUND() LOG_CONCAT2(LOG_DIR, "Hipe_3%N.log")


//DECLARE_string(log_backtrace_at);  // logging.cc
//FLAGS_log_backtrace_at = stack_print; 

#define PRINTSTACK(severity) \
{ \
	char stack_print[100]; \
	snprintf(stack_print, 100, "%s:%d", removeDirectoryName(__FILE__).c_str(), __LINE__); \
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
