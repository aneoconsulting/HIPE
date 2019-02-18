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
#include <functional>
#include <memory>
#include <algorithm>

class dtFunctor {
private:
	// have a placeholder base-type:
	struct base {
		virtual ~base() {};
	};

	// have a derived type to wrap the 
	//  actual functor object:
	template <typename Obj, typename... Args>
	struct wrapped : base {
		typedef std::function<void(Obj, Args...)> func_type;
		typedef Obj obj_type;

		func_type f;
		wrapped(func_type aFunc) : f(aFunc) { };

		wrapped(const wrapped & Func) : f(Func.f) { };
		wrapped(wrapped & Func) : f(Func.f) { };

		~wrapped() {}

	};
	// Hold a base-type pointer:
	std::unique_ptr<base> p_base;
public:
	dtFunctor() {};

	template <typename... Args>
	dtFunctor(dtFunctor & aFunc) 
	{
		p_base.release();
		p_base = std::unique_ptr<base>(aFunc.p_base.get()); 
	}

	template <typename... Args>
	dtFunctor(const dtFunctor & aFunc) 
	{
		p_base.release();
		p_base = std::unique_ptr<base>(aFunc.p_base.get());
	}

	// Initialize object with any function:
	template <typename... Args>
	dtFunctor(std::function<void(Args...)> aFunc) 		
	{
		p_base = std::unique_ptr<wrapped<Args...>>(new wrapped<Args...>(aFunc));
	};
	// Assign object with any function:
	template <typename... Args>
	dtFunctor& operator=(std::function<void(Args...)> aFunc) {
		p_base = std::unique_ptr<wrapped<Args...>>(new wrapped<Args...>(aFunc));
		return *this;
	};

	dtFunctor& operator=(dtFunctor & dtFunc) {
		p_base.release();
		p_base = std::unique_ptr<base>(dtFunc.p_base.get());
		return *this;
	};
	

	// Call the function by casting the base
	//  pointer back to its derived type:
	template <class Obj, typename... Args>
	void operator()(Obj obj, Args... args) const {
		wrapped<Obj, Args...>* p_wrapped = ( wrapped<Obj, Args...>*) (p_base.get()	);
		
		
		if (p_wrapped) // <- if cast successful
			p_wrapped->f(obj, args...);
		else
			throw std::runtime_error("Invalid arguments to function object call!");
	};

	~dtFunctor() {}
};
