#pragma once
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#define LOG_CONCAT2(a, b) a ## b

#ifdef WIN32
#define LOG_DIR "c:\\temp\\"
#else
#define LOG_DIR "/tmp/"
#endif

#define LOG_PATH() LOG_CONCAT2(LOG_DIR, "Hipe.log")
#define LOG_PATH_ROUND() LOG_CONCAT2(LOG_DIR, "Hipe_3%N.log")



namespace core
{


	typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> BoostLogger;


	class Logger
	{
	public:
		typedef boost::log::trivial::severity_level Level;


		BoostLogger boostLogger;
		Level level;

		Logger() : level(Level::info)
		{
			
		}

		Logger(BoostLogger logger) : boostLogger(logger), level(Level::info) {}
		

		template <typename T>
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

		static void init();
	
	
		
	};

	Logger setClassNameAttribute(const std::string& classNameIn);
}
