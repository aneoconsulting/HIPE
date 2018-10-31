//@HIPE_LICENSE@
//#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <pydata/pyImageData.h>

#pragma warning(push, 0)
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#pragma warning(pop)

#include <pydata/pyboostcvconverter.hpp>
#include <iostream>
#include "TupleConverter.h"
#include "pyShapeData.h"



namespace pbcvt {

    using namespace boost::python;

/**
 * Example function. Basic inner matrix product using explicit matrix conversion.
 * @param left left-hand matrix operand (NdArray required)
 * @param right right-hand matrix operand (NdArray required)
 * @return an NdArray representing the dot-product of the left and right operands
 */
    PyObject *dot(PyObject *left, PyObject *right) {

        cv::Mat leftMat, rightMat;
        leftMat = pbcvt::fromNDArrayToMat(left);
        rightMat = pbcvt::fromNDArrayToMat(right);
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        // Check that the 2-D matrices can be legally multiplied.
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;
        PyObject *ret = pbcvt::fromMatToNDArray(result);
        return ret;
    }

//This example uses Mat directly, but we won't need to worry about the conversion
/**
 * Example function. Basic inner matrix product using implicit matrix conversion.
 * @param leftMat left-hand matrix operand
 * @param rightMat right-hand matrix operand
 * @return an NdArray representing the dot-product of the left and right operands
 */
    cv::Mat dot2(cv::Mat leftMat, cv::Mat rightMat) {
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;

        return result;
    }

	//This function will force windows compiler to link with boost numpy
	static void force_numpy_kink()
	{
		   boost::python::numpy::initialize();
	}

#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();
		
        import_array();
#if (PY_VERSION_HEX >= 0x03000000)
        return NULL;
#endif
    }

    //BOOST_PYTHON_MODULE (pbcvt) {
    //    //using namespace XM;
    //    init_ar();

    //    //initialize converters
    //    to_python_converter<cv::Mat,
    //            pbcvt::matToNDArrayBoostConverter>();
    //    pbcvt::matFromNDArrayBoostConverter();

    //    //expose module-level functions


    //}

} //end namespace pbcvt

void print_arity(boost::python::object fn)
{
  std::size_t arity = boost::python::extract<std::size_t>(
                        fn.attr("func_code").attr("co_argcount"));
  std::cout << arity << std::endl;
}

namespace boost {
	namespace python {

		struct release_gil_policy
		{
			// Ownership of this argument tuple will ultimately be adopted by
			// the caller.
			template <class ArgumentPackage>
			static bool precall(ArgumentPackage const&)
			{
				// Release GIL and save PyThreadState for this thread here

				return true;
			}

			// Pass the result through
			template <class ArgumentPackage>
			static PyObject* postcall(ArgumentPackage const&, PyObject* result)
			{
				// Reacquire GIL using PyThreadState for this thread here

				return result;
			}

			typedef default_result_converter result_converter;
			typedef PyObject* argument_package;

			template <class Sig>
			struct extract_return_type : mpl::front<Sig>
			{
			};

		private:
			// Retain pointer to PyThreadState on a per-thread basis here

		};
	}
}

#ifdef BOOST_PYTHON_STATIC_LIB
#undef BOOST_PYTHON_STATIC_LIB
#endif


void export_cpptuple_conv() {
    create_tuple_converter<float, float>();
    create_tuple_converter<int, int>();
    create_tuple_converter<int, int, int, int>();
    create_tuple_converter<int, int, int, int, int, int, int, int>();
}

std::tuple<float, float> tupPoint2f(std::tuple<float, float> t){return t;}
std::tuple<int, int> tupPoint(std::tuple<float, float> t){return t;}
std::tuple<int, int, int, int> tupRect(std::tuple<int, int, int, int> t){return t;}
std::tuple<int, int, int, int, int, int, int, int>	tupQuad(std::tuple<int, int, int, int, int, int, int, int> t){return t;}


	BOOST_PYTHON_MODULE(pydata)
	{
		pbcvt::init_ar();
		export_cpptuple_conv();

		//initialize converters
		to_python_converter<cv::Mat,
			pbcvt::matToNDArrayBoostConverter>();
		pbcvt::matFromNDArrayBoostConverter();

		py::def("tupPoint2f", tupPoint2f);
		py::def("tupPoint", tupPoint);
		py::def("tupPoint", tupRect);

		def("dot", pbcvt::dot);
		def("dot2", pbcvt::dot2);

		boost::python::class_<pyImageData>("imageData")
			.def("assign", &pyImageData::assign)
			.def("set", &pyImageData::set)
			.def_readwrite("img", &pyImageData::get)
			.def("get", &pyImageData::get);

		boost::python::class_<pyShapeData>("shapeData")
    		.def("addRect", &pyShapeData::addRect)
    		.def("addQuad", &pyShapeData::addQuad);
	}