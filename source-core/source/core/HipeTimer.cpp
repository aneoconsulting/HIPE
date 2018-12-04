//@HIPE_LICENSE@

#include <core/HipeTimer.h>

namespace core
{
	void HipeTimer::start()
	{
		hipe_gettimeofday(&timeval_start, nullptr);
	}

	HipeTimer& HipeTimer::stop()
	{
		hipe_gettimeofday(&timeval_stop, nullptr);

		return *this;
	}

	std::string HipeTimer::getTimerString(std::string format)
	{
		return std::string("Not yet implemented");
	}

	double HipeTimer::getStartTimerInMili()
	{
		return static_cast<double>(timeval_start.tv_sec * 1000000L + timeval_start.tv_usec) / 1000L;
	}

	double HipeTimer::getStartTimerInMicro()
	{
		return static_cast<double>(timeval_start.tv_sec * 1000000L + timeval_start.tv_usec);
	}

	hipetimeval HipeTimer::getElapse()
	{
		elapse_ht.tv_sec = timeval_stop.tv_sec - timeval_start.tv_sec;
		elapse_ht.tv_usec = timeval_stop.tv_usec - timeval_start.tv_usec;

		return elapse_ht;
	}

	double HipeTimer::getStopTimerInMili()
	{
		elapse_ht.tv_sec = timeval_stop.tv_sec - timeval_start.tv_sec;
		elapse_ht.tv_usec = timeval_stop.tv_usec - timeval_start.tv_usec;

		return static_cast<double>(timeval_stop.tv_sec * 1000000L + timeval_stop.tv_usec) / 1000L;
	}

	double HipeTimer::getStopTimerInMicro()
	{
		elapse_ht.tv_sec = timeval_stop.tv_sec - timeval_start.tv_sec;
		elapse_ht.tv_usec = timeval_stop.tv_usec - timeval_start.tv_usec;

		return static_cast<double>(timeval_stop.tv_sec * 1000000L + timeval_stop.tv_usec);
	}

	double HipeTimer::getElapseTimeInMili()
	{
		double elapse = (timeval_stop.tv_sec * 1000000L + timeval_stop.tv_usec) - (timeval_start.tv_sec * 1000000L
			+ timeval_start.tv_usec);

		return elapse / 1000.0;
	}

	double HipeTimer::getElapseTimeInMicro()
	{
		double elapse = (timeval_stop.tv_sec * 1000000L + timeval_stop.tv_usec) - (timeval_start.tv_sec * 1000000L
			+ timeval_start.tv_usec);

		return elapse;
	}
}



