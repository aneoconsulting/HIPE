#pragma once
#include <iostream>

#include <functional>
#include <memory>

namespace core
{
	class InvokerBase;

	template<typename return_type, typename... params>
	class Invoker;

	class InvokerBase
	{

	protected:
		struct WrapperBase
		{

		};

		std::unique_ptr<WrapperBase> _wrap;
	public:


		InvokerBase() { }

		InvokerBase(InvokerBase & invoMethod)
		{
			_wrap.release();
			_wrap.reset(invoMethod._wrap.get());
		}

		InvokerBase(const InvokerBase & invoMethod)
		{
			_wrap.release();
			_wrap.reset(invoMethod._wrap.get());
		}

		template<typename return_type, typename... params>
		InvokerBase & operator=(Invoker<return_type, params...> invoMethod)
		{
			_wrap.release();
			_wrap.reset(invoMethod._wrap.get());
			return *this;
		}

		~InvokerBase()
		{
			_wrap.release();

		}
		template<typename return_type, typename... params>
		return_type operator()(void* callee, params... xs)
		{
			Invoker<return_type, params...> * invoMethod = static_cast<Invoker<return_type, params...>*>(this);

			return invoMethod->operator()(callee, xs...);
		}

		template<typename return_type, typename... params>
		Invoker<return_type, params...> & operator=(const Invoker<return_type, params...> & invoMethod)
		{
			_wrap.reset(invoMethod._wrap.get());
			return *this;
		}

		template<typename return_type, typename... params>
		Invoker<return_type, params...> & operator=(Invoker<return_type, params...> invoMethod)
		{
			_wrap.reset(invoMethod._wrap.get());
			return *this;
		}


		InvokerBase & operator=(const InvokerBase & invoMethod)
		{
			_wrap.reset(invoMethod._wrap.get());
			return *this;
		}
	};

	template<typename return_type, typename... params>
	class Invoker : public InvokerBase
	{
		typedef return_type(*Type)(void* callee, params...);

	protected:
		struct Wrapper : public WrapperBase
		{
			Type fpCallbackFunction;
			Wrapper(Type & function) : fpCallbackFunction(function) {}
			Wrapper() : fpCallbackFunction(nullptr) {}
		};

	public:
		Invoker()
		{
			_wrap.reset(nullptr);
		}

		Invoker(Type function)
		{
			_wrap.reset(new Wrapper(function));
		}

		Invoker(Invoker & right)

		{
			_wrap.release();
			Wrapper * wrap = static_cast<Wrapper*>(right._wrap.get());
			_wrap.reset(new Wrapper(wrap->fpCallbackFunction));
		}

		~Invoker()
		{
			_wrap.release();
		}

		void operator=(Invoker & right)
		{
			_wrap.release();

			Wrapper * wrap = static_cast<Wrapper*>(right._wrap.get());
			_wrap.reset(new Wrapper(wrap->fpCallbackFunction));
		}
		template <class T, return_type(T::*TMethod)(params...)>
		static Invoker<return_type, params...> for_function()
		{
			Invoker<return_type, params...> d(&methodInvoke<T, TMethod>);

			return d;
		}
		return_type operator()(void* callee, params... xs)
		{
			Wrapper *rel = static_cast<Wrapper *>(_wrap.get());
			return (*(rel->fpCallbackFunction))(callee, xs...);
		}

	private:

		template <class T, return_type(T::*TMethod)(params...)>
		static return_type methodInvoke(void* callee, params... xs)
		{
			T* p = static_cast<T*>(callee);
			return (p->*TMethod)(xs...);
		}


	};
}