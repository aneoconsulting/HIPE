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
#include <map>

namespace http
{
	class CommandManager
	{
	public:
		// For generic types that are functors, delegate to its 'operator()'
		template <typename T>
		struct function_traits
			: public function_traits<decltype(&T::operator())>
		{
		};

		// for pointers to member function
		template <typename ClassType, typename ReturnType, typename... Args>
		struct function_traits<ReturnType(ClassType::*)(Args ...) const>
		{
			//enum { arity = sizeof...(Args) };
			typedef std::function<ReturnType(Args ...)> f_type;
		};

		// for pointers to member function
		template <typename ClassType, typename ReturnType, typename... Args>
		struct function_traits<ReturnType(ClassType::*)(Args ...)>
		{
			typedef std::function<ReturnType(Args ...)> f_type;
		};

		// for function pointers
		template <typename ReturnType, typename... Args>
		struct function_traits<ReturnType(*)(Args ...)>
		{
			typedef std::function<ReturnType(Args ...)> f_type;
		};

		template <typename L>
		static typename function_traits<L>::f_type make_function(L l)
		{
			return (typename function_traits<L>::f_type)(l);
		}
		template <typename... Args>
		static bool callOption(std::string optionName, std::function<bool(std::string, Args...)> lamb, Args...args)
		{
			/*typedef function_traits<decltype(lamb)> traits;
			typename traits::f_type ff = lamb;
			return make_function(ff)(optionName, args...);*/
			return lamb(optionName, args...);
		}

		CommandManager()
		{
		}
	};
}
