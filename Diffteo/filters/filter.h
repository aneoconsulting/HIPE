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

using namespace std;
using namespace cv;

class filter
{
	virtual void apply_self() = 0;

protected:

	filter* parent;
	int value;

public:

	int nchild = 0;
	vector<filter*> children;

	// Constructor
	filter(int _val, filter* _parent = NULL);

	// Destructor
	~filter() {}

	// Methods
	void set(int _val);
	void print();
	void declare_as_child(filter* _parent);
	void apply();
};

class blur_filter : public filter
{
	Mat& input;
	Mat& output;
	Size2i ksize;
	double sigmaX, sigmaY;
	int borderType;

	void apply_self();

public:
	blur_filter(int _val, filter* _parent, Mat& _input, Mat& _output, int _size, double _sigmaX, double _sigmaY = 0, int _borderType = BORDER_DEFAULT);
};

class gray_filter : public filter
{
	Mat& input;
	Mat& output;
	int mode;

	void apply_self();
public:
	gray_filter(int _val, filter* _parent, Mat& _input, Mat& _output, int mode = COLOR_BGR2GRAY);
};


class threshold_filter : public filter
{
	void apply_self();
public:
	threshold_filter(int _val, filter* _parent = NULL);
};

class detourrage_filter : public filter
{
	void apply_self();
public:
	detourrage_filter(int _val, filter* _parent = NULL);
};