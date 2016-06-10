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

/************* BASE ABSTRACT filter *****************/
filter::filter(filter* parent)
{
	_parent = parent;
	if (_parent != NULL)
		declare_as_child(_parent);
}
filter::~filter(){ if (_isRoot) delete _parent; }

void filter::declare_as_child(filter* parent)
{
	parent->_nchild++;
	parent->_children.push_back(this);
}
void filter::apply()
{
	apply_self();
	for (int i = 0; i < _nchild; i++)
		(*_children[i]).apply();
}

/************* ROOT filter **************************/
root_filter::root_filter(Mat& input) : filter(NULL)
{
	_isRoot = true;
	_outputs.push_back(input);
}

/************* GRAY filter **************************/
gray_filter::gray_filter(filter* parent, int mode)
	: filter(parent)
{
	_mode = mode;
}

gray_filter::gray_filter(Mat& input, int mode)
	: filter(NULL)
{
	_mode = mode;

	_parent = new root_filter(input);
}

void gray_filter::apply_self()
{
	Mat l_input;
	if (_parent)
		l_input = _parent->output();
	else
		THROW_EXCEPTION(-1, "Cannot take parent filter output as input");

	Mat l_output(l_input.size(), l_input.type());
	cvtColor(l_input, l_output, _mode); 
	ShowImage(l_output);
	
	if (_nchild != 0)
		_outputs.push_back(l_output);
	else
		_parent->_outputs.push_back(l_output);
}

/************* BLUR filter **************************/

blur_filter::blur_filter(filter* parent, int size, double sigmaX, double sigmaY, int borderType)
	: filter(parent)
{
	_ksize.height = _ksize.width = size;
	_sigmaX = sigmaX;
	_sigmaY = sigmaY;
	_borderType = borderType;
}

blur_filter::blur_filter(Mat& input, int size, double sigmaX, double sigmaY, int borderType)
	: filter(NULL)
{
	_ksize.height = _ksize.width = size;
	_sigmaX = sigmaX;
	_sigmaY = sigmaY;
	_borderType = borderType;

	_parent = new root_filter(input);
}

void blur_filter::apply_self()
{
	Mat l_input;
	if (_parent)
		l_input = _parent->output();
	else
		THROW_EXCEPTION(-1, "Cannot take parent filter output as input");

	Mat l_output(l_input.size(), l_input.type());
	GaussianBlur(l_input, l_output, _ksize, _sigmaX, _sigmaY, _borderType);
	ShowImage(l_output);
	
	if (_nchild != 0)
		_outputs.push_back(l_output);
	else
		_parent->_outputs.push_back(l_output);
}

/************* THRESHOLD filter **************************/

threshold_filter::threshold_filter(filter* parent, double thresh, double maxval, int type)
	: filter(parent) 
{
	_thresh = thresh;
	_maxval = maxval;
	_type = type;
}

threshold_filter::threshold_filter(Mat& input, double thresh, double maxval, int type)
	: filter(NULL)
{
	_thresh = thresh;
	_maxval = maxval;
	_type = type;

	_parent = new root_filter(input);
}

void threshold_filter::apply_self()
{
	Mat l_input;
	if (_parent)
		l_input = _parent->output();
	else
		THROW_EXCEPTION(-1, "Cannot take parent filter output as input");

	Mat l_output(l_input.size(), l_input.type());
	threshold(l_input, l_output, _thresh, _maxval, _type);
	ShowImage(l_output);

	if (_nchild != 0)
		_outputs.push_back(l_output);
	else
		_parent->_outputs.push_back(l_output);
}