#pragma once
#include <core/Invoker.h>
#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/property_tree/ptree.hpp>

#define TO_STR(A) #A

#define CONNECTOR(DataIn, DataOut)\
	data::ConnexData<DataIn,DataOut> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define CONNECTOR_OUT(DataOut)\
	data::ConnexOutput<DataOut> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define CONNECTOR_IN(DataIn)\
	data::ConnexInput<DataIn> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define REGISTER(Constructor, params)\
	public:\
	\
	typedef ProxyFunctor<Constructor> _proxyFunctor; \
	typedef Constructor mytype;\
	Constructor params : IFilter(#Constructor)


#define EXPAND_VAR(elem) elem
#define CONCAT(a,b) a ## b
#define CONCAT2(a, b) CONCAT( a , b )
#define CONCAT3(a, b, c) CONCAT2(CONCAT2(a,b), c)
#define CONCAT4(a, b, c, d) CONCAT2(CONCAT3(a,b, c), d)
#define CONCAT5(a, b, c, d, e) CONCAT2(CONCAT4(a,b, c, d), e)
#define CONCAT2_STR(a, b) TO_STR(a ## b)
#define CONCAT3_STR(a, b, c) TO_STR(a ## b ## c)

#define STRINGIZE(x) x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

#define REGISTER_P(typef, params)\
		typef params; \
		typedef typef vartype__##params; \
		public: \
		void set_##params##_from_json(boost::property_tree::ptree & jsonClass) \
		{\
				this-> params = jsonClass.get<typef>(std::string (#params));\
		}\
		void set_##params(vartype__##params & __##params) \
		{\
				this-> params = __##params;\
		}\
		vartype__##params get_##params() { return this-> params; }\
		void copy_##params(mytype * instance) \
		{\
				this-> params = instance->params;\
		}

#define ADD_ARGS(r, classname, elem)\
	core::InvokerBase CONCAT4(invoke_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<void, CONCAT2(classname::vartype__, elem)&>::for_function<classname,  & CONCAT2(classname::set_, elem) >();\
	CONCAT4(std::string str_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(set_ , elem)), CONCAT4(invoke_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT5(invoke_set_, classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME) ) = core::Invoker<void, boost::property_tree::ptree &>::for_function<classname, & CONCAT3(classname::set_, elem, _from_json)>();\
	CONCAT5(std::string str_ , classname ## _ , elem, _from_json, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT3_STR(set_ , elem , _from_json)), CONCAT5(invoke_set_ , classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT4(invoke_get_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<CONCAT2(classname::vartype__, elem)>::for_function<classname, & CONCAT2(classname::get_, elem)>();\
	CONCAT4(std::string str_get_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(get_ , elem)), CONCAT4(invoke_get_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT4(invoke_copy_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<void, classname *>::for_function<classname, & CONCAT2(classname::copy_, elem)>();\
	CONCAT4(std::string str_copy_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(copy_ , elem)), CONCAT4(invoke_copy_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\


#define ADD_CLASS(classname, ...)\
	std::string CONCAT3(str_constructor_, classname , EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __invoke(instance, function, ...) 	RegisterTable::getInstance().invoke(instance, function, __VA_ARGS__)

