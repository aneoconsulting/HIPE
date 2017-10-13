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
