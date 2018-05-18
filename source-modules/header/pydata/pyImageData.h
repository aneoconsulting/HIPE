//@HIPE_LICENSE@
#pragma once

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <boost/python.hpp>
#pragma warning(pop)

#include <pydata/pyboostcvconverter.hpp>
using namespace boost::python;

class pyImageData
{
	cv::Mat img;
	//filter::data::ImageData & _data;

public:
	pyImageData()
	{
		
	}
	
	pyImageData(data::ImageData & data) 
	{
		img = data.getMat();
	}

	cv::Mat get()
	{
		return img;
	}

	void assign(cv::Mat input)
	{
		img = input;
	}
	
	void set(int i, int y)
	{
		
	}

	
};
