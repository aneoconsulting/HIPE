//@HIPE_LICENSE@
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
