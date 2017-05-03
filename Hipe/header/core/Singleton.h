#pragma once
#include <mutex>

class BaseSingleton
{
protected:
	
	
};


template<class T>
class Singleton : public BaseSingleton
{
protected:
	static T * _instance;
	static std::mutex _mutex;

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
