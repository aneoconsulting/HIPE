#include "misc.h"
#include <vector>

using namespace std;
using namespace cv;

class filter
{
	friend class blur_filter;
	friend class threshold_filter;
	friend class gray_filter;

	virtual void apply_self() = 0;
	virtual Mat& output() = 0;

protected:
	filter* _parent;
	vector<Mat> _outputs;
	bool _isRoot = false;

public:
	int _nchild = 0;
	vector<filter*> _children;

	// Constructor
	filter(filter* parent = NULL);
	// Destructor
	~filter();

	// Methods
	void declare_as_child(filter* parent);
	void apply();
};

class root_filter : public filter
{
public:
	root_filter(Mat & input);

	inline Mat & output() { return _outputs[0]; }
	inline void apply_self() {};
};

class blur_filter : public filter
{
	void apply_self();

	// Parameters of the grey filter operation
	Size2i _ksize;
	double _sigmaX, _sigmaY;
	int _borderType;

public:
	blur_filter(filter* parent, int size, double sigmaX, double sigmaY = 0, int borderType = BORDER_DEFAULT);
	blur_filter(Mat& input, int size, double sigmaX, double sigmaY = 0, int borderType = BORDER_DEFAULT);

	inline Mat & output() { return _outputs[_outputs.size() -1]; }
};

class gray_filter : public filter
{
	void apply_self();

	// Parameters of the grey filter operation
	int _mode;

public:
	gray_filter(filter* parent, int mode = COLOR_BGR2GRAY);
	gray_filter(Mat& input, int mode = COLOR_BGR2GRAY);

	inline Mat & output() { return _outputs[_outputs.size() - 1]; }
};

class threshold_filter : public filter
{
	void apply_self();

	// Parameters of the threshold filter operation
	double _thresh;
	double _maxval;
	int _type;

public:
	threshold_filter(filter* parent, double thresh = 0, double maxval = 255, int type = THRESH_OTSU);
	threshold_filter(Mat& input, double thresh = 0, double maxval = 255, int type = THRESH_OTSU);

	inline Mat & output() { return _outputs[_outputs.size() - 1]; }
};