//@HIPE_LICENSE@
#include <core/Logger.h>
#include <glog/raw_logging.h>

#pragma warning(push, 0)
#pragma warning(pop)

#include <fstream>

namespace core
{

	void Logger::init(const char *binary_name)
	{
		//static HipeGlogSink *tcpSink = new HipeGlogSink();
		FLAGS_alsologtostderr = true;
		google::InitGoogleLogging(binary_name);
		//google::InstallFailureSignalHandler();
		//google::AddLogSink(tcpSink);
		

		FLAGS_alsologtostderr = true;
		
		//typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

		//boost::shared_ptr<text_sink> m_sink(new text_sink);

		//boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::null_deleter());
		//boost::shared_ptr<std::ostream> stream_file(new std::ofstream(LOG_PATH(), std::ostream::app));
		//m_sink->locked_backend()->add_stream(stream_out);
		//m_sink->locked_backend()->add_stream(stream_file);

		//m_sink->locked_backend()->auto_flush(true);

		//m_sink->set_formatter
		//	(
		//		boost::log::expressions::stream
		//		<< "["
		//		<< boost::log::expressions::attr<boost::posix_time::ptime>("TimeStamp") << "] "
		//		<< "[" << boost::log::trivial::severity << "] | "
		//		<< boost::log::expressions::attr<std::string>("ClassName") << " | : "
		//		<< boost::log::expressions::message
		//	);

		//boost::log::core::get()->add_sink(m_sink);

		//boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
		//// Set up our sink and formatting
		//boost::log::add_file_log(
		//	boost::log::keywords::file_name = LOG_PATH_ROUND(),
		//	boost::log::keywords::rotation_size = 1 * 1024 * 1024,
		//	boost::log::keywords::max_size = 20 * 1024 * 1024,
		//	boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
		//	boost::log::keywords::auto_flush = true);

	}
}
