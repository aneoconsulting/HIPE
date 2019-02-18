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



