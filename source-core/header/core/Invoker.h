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
