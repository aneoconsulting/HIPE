#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"

#include <iostream>
#include <vector>
#include "misc.h"
#include "filter.h"

using namespace std;
using namespace cv;


/*** Implementation of filter base-class methods ***/

filter::filter(int _val, filter* _parent)
{
	parent = _parent;
	value = _val;
	if (_parent != NULL)
		declare_as_child(_parent);
}

void filter::set(int _val) { value = _val; }
void filter::print() { cout << "Value is " << value << endl; }
void filter::declare_as_child(filter* _parent)
{
	_parent->nchild++;
	_parent->children.push_back(this);
}
void filter::apply()
{
	for (int i = 0; i < nchild; i++)
		(*children[i]).apply();
	apply_self();
}


/*** Dervied class : blur filter ***/

blur_filter::blur_filter(int _val, filter* _parent, Mat& _input, Mat& _output, int _size, double _sigmaX, double _sigmaY, int _borderType)
	: input(_input), output(_output), filter(_val, _parent)
{
	ksize.height = ksize.width = _size;
	sigmaX = _sigmaX;
	sigmaY = _sigmaY;
	borderType = _borderType;
}
void blur_filter::apply_self()
{
	GaussianBlur(input, output, ksize, sigmaX, sigmaY, borderType); ShowImage(output);
}


/*** Dervied class : grey filter ***/

gray_filter::gray_filter(int _val, filter* _parent, Mat& _input, Mat& _output, int _mode)
	: input(_input), output(_output), filter(_val, _parent)
{
	mode = _mode;
}
void gray_filter::apply_self()
{
	cvtColor(input, output, COLOR_BGR2GRAY); ShowImage(output);
}



/*** Dervied class : other (dummy) filters ***/

void threshold_filter::apply_self() { cout << "I'm a threshold filter !  (adress) " << this << "\t value = " << value << endl; }
threshold_filter::threshold_filter(int _val, filter* _parent) : filter(_val, _parent) {}

void detourrage_filter::apply_self() { cout << "I'm a detourrage filter !  (adress)" << this << "\t value = " << value << endl; }
detourrage_filter::detourrage_filter(int _val, filter* _parent) : filter(_val, _parent) {}