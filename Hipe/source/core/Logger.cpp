#include <core/Logger.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace core
{
	Logger setClassNameAttribute(const std::string& classNameIn)
	{
		BoostLogger lg;
		lg.add_attribute("ClassName", boost::log::attributes::constant<std::string>(classNameIn));
		Logger finalLg(lg);

		return finalLg;
	}
	
}