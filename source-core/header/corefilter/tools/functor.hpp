//@HIPE_LICENSE@
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
