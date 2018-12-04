//@HIPE_LICENSE@
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
