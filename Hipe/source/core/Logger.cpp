#include <core/Logger.h>

#pragma warning(push, 0)

#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/core/null_deleter.hpp>
#pragma warning(pop)

#include <fstream>

namespace core
{
	namespace logging = boost::log;
	namespace src = boost::log::sources;
	namespace sinks = boost::log::sinks;
	namespace keywords = boost::log::keywords;

	Logger setClassNameAttribute(const std::string& classNameIn)
	{
		BoostLogger lg;
		lg.add_attribute("ClassName", boost::log::attributes::constant<std::string>(classNameIn));
		Logger finalLg(lg);

		return finalLg;
	}

	void Logger::init()
	{
		typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

		boost::shared_ptr<text_sink> m_sink(new text_sink);

		boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::null_deleter());
		boost::shared_ptr<std::ostream> stream_file(new std::ofstream(LOG_PATH(), std::ostream::app));
		m_sink->locked_backend()->add_stream(stream_out);
		m_sink->locked_backend()->add_stream(stream_file);

		m_sink->locked_backend()->auto_flush(true);

		m_sink->set_formatter
			(
				boost::log::expressions::stream
				<< "["
				<< boost::log::expressions::attr<boost::posix_time::ptime>("TimeStamp") << "] "
				<< "[" << boost::log::trivial::severity << "] | "
				<< boost::log::expressions::attr<std::string>("ClassName") << " | : "
				<< boost::log::expressions::message
			);

		boost::log::core::get()->add_sink(m_sink);

		boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
		// Set up our sink and formatting
		boost::log::add_file_log(
			boost::log::keywords::file_name = LOG_PATH_ROUND(),
			boost::log::keywords::rotation_size = 1 * 1024 * 1024,
			boost::log::keywords::max_size = 20 * 1024 * 1024,
			boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
			boost::log::keywords::auto_flush = true);

	}
}
