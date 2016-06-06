#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"

#include <iostream>
#include "filter.h"
#include "scheduler.h"
#include "misc.h"
#include "tests.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
	// Test functions are located in tests.cpp
	int err1 = test1();
	int err2 = test2(argc, argv);

	cin.get();
	if (err1 == 0 && err2 == 0)
		return 0;
}