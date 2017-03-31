#pragma once


#include <boost/preprocessor.hpp>
#include <filter/tools/functor.hpp>
#include <filter/tools/RegisterTable.h>
#include <boost/property_tree/ptree.hpp>

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
		  return _proxyFunctor::getMethodSetter<typef>(dtFunc_##params, &mytype::set_##params); \
		}\
		static int getsetter_##params##_from_json(dtFunctor & dtFunc_)\
		{\
			return _proxyFunctor::getMethodSetter<boost::property_tree::ptree &>(dtFunc_, &mytype::set_##params##_from_json);\
		}\
		static int getgetter_##params(dtFunctor & dtFunc_##params)\
		{\
			return _proxyFunctor::getMethodGetter<typef>(dtFunc_##params, &mytype::get_##params);\
		}

#define TO_STR(A) #A

#define REGISTER(Constructor, params)\
	public:\
	typedef ProxyFunctor<Constructor> _proxyFunctor; \
	typedef Constructor mytype;\
	Constructor params : IFilter(#Constructor)

#define EXPAND_VAR(elem) elem
#define CONCAT(a,b) a ## b
#define CONCAT2(a, b) CONCAT( a , b )
//#define CONCAT3(a, b, c) CONCAT2(a, b) ## c // does not work with gcc : error: pasting ")" and "[une variable du code C]" does not give a valid preprocessing token 
#define CONCAT3(a, b, c) CONCAT2(CONCAT2(a,b), c)
#define CONCAT4(a, b, c, d) CONCAT2(CONCAT3(a,b, c), d)
#define CONCAT5(a, b, c, d, e) CONCAT2(CONCAT4(a,b, c, d), e)
#define CONCAT2_STR(a, b) TO_STR(a ## b)
#define CONCAT3_STR(a, b, c) TO_STR(a ## b ## c)

#define STRINGIZE(x) x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

#define ADD_ARGS(r, classname, elem)\
	dtFunctor CONCAT4(dtFunc_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  ;\
	int CONCAT4(err_ , classname , elem, EXPAND_VAR(FILE_BASENAME) ) = CONCAT2(classname::getsetter_ , elem) (CONCAT4(dtFunc_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	dtFunctor CONCAT5(dtFunc_set_ , classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME) );\
	int CONCAT5(err_ , classname , elem, _from_json, EXPAND_VAR(FILE_BASENAME)) = CONCAT3(classname::getsetter_ , elem, _from_json) (CONCAT5(dtFunc_set_ , classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME)));\
	CONCAT4(std::string str_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(set_ , elem)), CONCAT4(dtFunc_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	CONCAT5(std::string str_ , classname ## _ , elem, _from_json, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT3_STR(set_ , elem , _from_json)), CONCAT5(dtFunc_set_ , classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME)));\


//#define ADD_CLASS(classname, ...)\
//	std::string str_constructor_##classname = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
//	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\

#define ADD_CLASS(classname, ...)\
	std::string CONCAT3(str_constructor_, classname , EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

