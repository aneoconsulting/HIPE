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
#include <core/misc.h>


namespace core
{

	class HipeTimer
	{
	public:
		hipetimeval timeval_start;
		hipetimeval timeval_stop;

		hipetimeval elapse_ht;
		

	public:
		HipeTimer() {};

		/**
		 * \brief Start timer
		 */
		void start();

		/**
		 * \brief Stop timer
		 */
		HipeTimer& stop();

		std::string getTimerString(std::string format);

		/**
		 * \brief The start time from epcoch in miliseconds
		 * \return The  start time from epcoch in miliseconds
		 */
		double getStartTimerInMili();

		/**
		 * \brief The start time from epcoch in microseconds
		 * \return The start time from epcoch in microseconds
		 */
		double getStartTimerInMicro();


		hipetimeval getElapse();

		/**
		 * \brief The stop time from epcoch in milliseconds
		 * \return The stop time from epcoch in milliseconds
		 */
		double getStopTimerInMili();

		/**
		* \brief The stop time from epcoch in microseconds
		* \return The stop time from epcoch in microseconds
		*/
		double getStopTimerInMicro();

		/**
		 * \brief Get the time elapesed between call start and stop function
		 * \return the difference between the 2 values in miliseconds
		 */
		double getElapseTimeInMili();

		/**
		* \brief Get the time elapesed between call start and stop function
		* \return the difference between the 2 values in microseconds
		*/
		double getElapseTimeInMicro();
	};
}
