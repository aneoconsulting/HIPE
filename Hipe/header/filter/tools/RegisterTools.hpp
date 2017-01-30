#pragma once


#include <boost/preprocessor.hpp>
#include <filter/tools/functor.hpp>
#include <filter/tools/RegisterTable.h>

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
		std::function<ret(_myC*)> func = static_cast<std::function<ret(_myC*)>>(std::mem_fn(_Pmd));

		//dtFunc = [&value, &func, _Pmd]{ value = func(_Pmd); };

		return 0;
	}
};


#define REGISTER_P(typef, params)\
		typef params; \
		public: \
		void set_##params##_from_json(boost::property_tree::ptree & jsonClass) \
		{\
				this-> params = jsonClass.get<typef>(std::string (#params));\
		}\
		void set_##params(typef __##params) \
		{\
				this-> params = __##params;\
		}\
		typef get_##params() { return this-> params; }\
		static int getsetter_ ## params (dtFunctor & dtFunc_ ## params)\
		{\
			return _proxyFunctor::getMethodSetter<typef>(dtFunc_##params, &set_##params);\
		}\
		static int getsetter_##params##_from_json(dtFunctor & dtFunc_)\
		{\
			return _proxyFunctor::getMethodSetter<boost::property_tree::ptree>(dtFunc_, &set_##params##_from_json);\
		}\
		static int getgetter_##params(dtFunctor & dtFunc_##params)\
		{\
			return _proxyFunctor::getMethodGetter<typef>(dtFunc_##params, &get_##params);\
		}

#define TO_STR(A) #A

#define REGISTER(Constructor, params)\
	public:\
	typedef ProxyFunctor<Constructor> _proxyFunctor; \
	Constructor params : IFilter(std::string(#Constructor))

#define EXPAND_VAR(elem) elem
#define CONCAT2(a, b) a ## b
//#define CONCAT3(a, b, c) CONCAT2(a, b) ## c // does not work with gcc : error: pasting ")" and "[une variable du code C]" does not give a valid preprocessing token 
#define CONCAT3(a, b, c) a ## b ## c
#define CONCAT4(a, b, c, d) a ## b ## c ## d
#define CONCAT2_STR(a, b) TO_STR(a ## b)
#define CONCAT3_STR(a, b, c) TO_STR(a ## b ## c)

//#define ADD_ARGS(r, classname, elem)\
//	CONCAT3(classname::func_set_,elem, _t) CONCAT2(classname::func_set_, elem) = CONCAT3(classname::func_set_, elem , _t) (std::mem_fn(& CONCAT2(classname::set_, elem)));\
//	CONCAT2(dtFunctor dtFunc_set_ , elem) = CONCAT2(classname::func_set_ , elem);\
//	CONCAT2(std::string str_ , elem) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(set_ , elem)), CONCAT2(dtFunc_set_ , elem));\

#define ADD_ARGS(r, classname, elem)\
	dtFunctor CONCAT3(dtFunc_set_ , classname, elem);\
	int CONCAT3(err_ , classname , elem) = CONCAT2(classname::getsetter_ , elem) (CONCAT3(dtFunc_set_ , classname, elem));\
	dtFunctor CONCAT4(dtFunc_set_ , classname, elem, _from_json);\
	int CONCAT4(err_ , classname , elem, _from_json) = CONCAT3(classname::getsetter_ , elem, _from_json) (CONCAT4(dtFunc_set_ , classname, elem, _from_json));\
	CONCAT3(std::string str_ , classname ## _ , elem) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(set_ , elem)), CONCAT3(dtFunc_set_ , classname, elem));\
	CONCAT4(std::string str_ , classname ## _ , elem, _from_json) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT3_STR(set_ , elem , _from_json)), CONCAT4(dtFunc_set_ , classname, elem, _from_json));\


//#define ADD_CLASS(classname, ...)\
//	std::string str_constructor_##classname = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
//	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\

#define ADD_CLASS(classname, ...)\
	std::string str_constructor_##classname = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

