#pragma once


//@HIPE_LICENSE@
#pragma once


#pragma warning(push, 0)
#include <boost/python.hpp>
#pragma warning(pop)

#include <pydata/pyboostcvconverter.hpp>
using namespace boost::python;

class pyShapeData
{
	std::vector<cv::Vec3f> _circlesArray;	//<! container of all the circles. The data are handled by cv::Vec3f objects
	std::vector<cv::Rect> _rectsArray;		//<! container of all the rectangles. The data are handled by cv::Rect objects
	std::vector<std::vector<cv::Point2f> > _quadrilatere;
	std::vector<std::vector<cv::Point> > _freeshape;		//<! container of all the free shape. The data are handled by std::vector<cv::Point2f> objects

	std::vector<std::string> _ids;				//<! container of indexed list of text for shape to dispose with it. 
	std::vector<cv::Scalar> _colors;			//<! container of indexed list to _freeShape of color for shape to dispose. 
	cv::Mat _refFrame;


public:
	std::vector<std::vector<cv::Point2f> > quadrilatere() const
	{
		return _quadrilatere;
	}

	void set_quadrilatere(std::vector<std::vector<cv::Point2f>> points)
	{
		_quadrilatere = points;
	}

	std::vector<cv::Vec3f> circles_array() const
	{
		return _circlesArray;
	}

	void set_circles_array(std::vector<cv::Vec3f> vecs)
	{
		_circlesArray = vecs;
	}

	std::vector<cv::Rect> rects_array() const
	{
		return _rectsArray;
	}

	void set_rects_array(std::vector<cv::Rect> rects)
	{
		_rectsArray = rects;
	}

	std::vector<std::vector<cv::Point>> freeshape() const
	{
		return _freeshape;
	}

	void set_freeshape(std::vector<std::vector<cv::Point>> pointses)
	{
		_freeshape = pointses;
	}

	std::vector<std::string> ids() const
	{
		return _ids;
	}

	void set_ids(std::vector<std::string> basic_strings)
	{
		_ids = basic_strings;
	}

	std::vector<cv::Scalar> colors() const
	{
		return _colors;
	}

	void set_colors(std::vector<cv::Scalar> scalars)
	{
		_colors = scalars;
	}

	cv::Mat ref_frame() const
	{
		return _refFrame;
	}

	void set_ref_frame(cv::Mat mat)
	{
		_refFrame = mat;
	}


public:
	pyShapeData() /* : _data(data::ShapeData())*/
	{
		
	}
	
	/*pyShapeData(data::ShapeData & data) 
	{
		_data = data;
	}
*/
	
	
	void addPointList(std::tuple<int, int> pydata)
	{
		std::cout << "Pydata : x " << std::get<0>(pydata) << " y " << std::get<1>(pydata) << std::endl;
		//_pointsArray.push_back(cv::Point(std::get<0>(pydata), std::get<1>(pydata)));
	}

	void addPoint2fList(std::tuple<float, float> pydata)
	{
		std::cout << "PyFloatData : x " << std::get<0>(pydata) << " y " << std::get<1>(pydata) << std::endl;
	}

	void addRect(std::tuple<int, int, int, int> pydata)
	{
		std::cout << "PyFloatData : x " << std::get<0>(pydata) << " y " << std::get<1>(pydata) << std::endl;
		_rectsArray.push_back(cv::Rect(std::get<0>(pydata), std::get<1>(pydata), std::get<2>(pydata), std::get<3>(pydata)));
	}

	void addQuad(std::tuple<int, int, int, int, int, int, int, int> pydata)
	{
		//_rectsArray.push_back(cv::Rect(std::get<0>(pydata), std::get<1>(pydata), std::get<2>(pydata), std::get<3>(pydata)));
		std::vector<cv::Point2f> quad;
		cv::Point2f topLeft(std::get<0>(pydata), std::get<1>(pydata)); quad.push_back(topLeft);
		cv::Point2f topRight(std::get<2>(pydata), std::get<3>(pydata)); quad.push_back(topRight);
		cv::Point2f downLeft(std::get<4>(pydata), std::get<5>(pydata)); quad.push_back(downLeft);
		cv::Point2f downRight(std::get<6>(pydata), std::get<7>(pydata)); quad.push_back(downRight);
		
		_quadrilatere.push_back(quad);
	}
};
