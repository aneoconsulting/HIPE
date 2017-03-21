#pragma once
#include <mutex>

class BaseSingleton
{
protected:
	static std::mutex _mutex;
	
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
		_mutex.lock();
		if (_instance == nullptr)
		{
			_instance = new T(args...);
		}
		_mutex.unlock();

		return _instance;
	}

};
