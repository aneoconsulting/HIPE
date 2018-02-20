#pragma once

#include <data/ImageData.h>


#include <boost/python.hpp>
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
