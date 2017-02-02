#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <ostream>
#include <fstream>

#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/utility/empty_deleter.hpp>

namespace core
{
	namespace logging = boost::log;
	namespace src = boost::log::sources;
	namespace sinks = boost::log::sinks;
	namespace keywords = boost::log::keywords;

	typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> BoostLogger;

	class Logger
	{
	public:
		typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
		typedef logging::trivial::severity_level Level;

	public:
		BoostLogger boostLogger;
		Level level;

		Logger() : level(Level::info)
		{
			
		}
		Logger(BoostLogger logger) : boostLogger(logger), level(Level::info) {}
		

		template< typename T >
		Logger& operator<<(const T& value)
		{
			BOOST_LOG_SEV(boostLogger, level) << value;
			level = Level::info;

			return *this;
		}

		Logger& operator<<(Level iLevel)
		{
			level = iLevel;

			return *this;
		}

		static void init()
		{
			boost::shared_ptr<text_sink> m_sink(new text_sink);

			boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::empty_deleter());
			boost::shared_ptr<std::ostream> stream_file(new std::ofstream("c:\\temp\\Hipe.log", std::ostream::app));
			m_sink->locked_backend()->add_stream(stream_out);
			m_sink->locked_backend()->add_stream(stream_file);

			m_sink->locked_backend()->auto_flush(true);

			m_sink->set_formatter
				(
				boost::log::expressions::stream
				<< "["
				<< boost::log::expressions::attr< boost::posix_time::ptime >("TimeStamp") << "] "
				<< "[" << boost::log::trivial::severity << "] | "
				<< boost::log::expressions::attr<std::string>("ClassName") << " | : "
				<< boost::log::expressions::message
				);

			boost::log::core::get()->add_sink(m_sink);

			boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
			// Set up our sink and formatting
			boost::log::add_file_log(
				boost::log::keywords::file_name = "c:\\temp\\Hipe_%3N.log",
				boost::log::keywords::rotation_size = 1 * 1024 * 1024,
				boost::log::keywords::max_size = 20 * 1024 * 1024,
				boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
				boost::log::keywords::auto_flush = true);

			
			//logging::core::get()->set_filter
			//	(
			//	logging::trivial::severity >= logging::trivial::info
			//	);

			//logging::add_common_attributes();
		}
	
		
	};

	Logger setClassNameAttribute(const std::string& classNameIn);
}
