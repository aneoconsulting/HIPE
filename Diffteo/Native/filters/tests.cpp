#include "filter.h"
#include "scheduler.h"
#include "misc.h"


/***
		Test 1 : Run actual filters on the image
		Following a tree structure
***/
int test1(int argc, char** argv)
{
	cout << "\n ***** Test 1 : Scheduler run over actual filters ***** \n\n";

	// Load image to apply filters on
	Mat ref_raw = getImg(argv[1]);
	if (ref_raw.empty())
	{
		cerr << "[WARNING] : No query image passed as a 3rd argument." << endl;
		return -1;
	}

	ShowImage(ref_raw);

	gray_filter gray(ref_raw);
	blur_filter blur0(&gray, 9, 3.5);
	threshold_filter thresh(&blur0, 0, 255, THRESH_OTSU);
	blur_filter blur1(&blur0, 9, 3.5);
	
	scheduler<filter> sched;
	sched.enqueue(&gray);
	sched.run();

	return 0;
}