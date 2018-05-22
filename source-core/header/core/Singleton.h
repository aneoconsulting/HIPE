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
#include <mutex>

#if defined _WIN32 || defined __CYGWIN__
#ifdef SINGLETON_BUILD
#ifdef __GNUC__
#define SINGLETON_EXPORT __attribute__ ((dllexport))
#else
#define SINGLETON_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define SINGLETON_EXPORT __attribute__ ((dllimport))
#else
#define SINGLETON_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define SINGLETON_LOCAL
#else
#if __GNUC__ >= 4
#define SINGLETON_EXPORT __attribute__ ((visibility ("default")))
#define SINGLETON_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define SINGLETON_EXPORT
#define SINGLETON_LOCAL
#endif
#endif

class BaseSingleton
{
protected:
	
	
};


template<class T>
class Singleton : public BaseSingleton
{
protected:
	static T * _instance;

public:
	template <typename... Args>
	static T *getInstance(Args...args)
	{
		static std::mutex _mutex;

		_mutex.lock();
		if (_instance == nullptr)
		{
			_instance = new T(args...);
		}
		_mutex.unlock();

		return _instance;
	}

};
