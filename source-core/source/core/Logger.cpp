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
