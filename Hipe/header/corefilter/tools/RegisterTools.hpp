#pragma once


#include <corefilter/tools/functor.hpp>
#include <corefilter/tools/filterMacros.h>

template <class _myC>
class ProxyFunctor
{
public:
	template<typename ret, typename _Rx>
	static int getMethodSetter(dtFunctor & dtFunc, _Rx _myC::*const _Pmd)
	{
		//_myC::func_set_, elem, _t)
		std::function<void(_myC*, ret)> func = static_cast<std::function<void(_myC*, ret)>>(std::mem_fn(_Pmd));
		dtFunc = func;
		return 0;
	}

	template<typename ret, typename _Rx>
	static int getMethodGetter(dtFunctor & dtFunc, _Rx _myC::*const _Pmd)
	{
		//_myC::func_set_, elem, _t)
		std::function<void(_myC*, ret)> func = static_cast<std::function<void(_myC*, ret)>>(std::mem_fn(_Pmd));
		dtFunc = func;
		return 0;
	}
	
};

