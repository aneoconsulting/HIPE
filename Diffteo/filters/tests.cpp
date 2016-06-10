#include <iostream>
#include "filter.h"
#include "scheduler.h"
#include "misc.h"


/***
		Test 1 : instantiate tree-structure of "dummy" filters
		and feed them to a scheduler. Run scheduler.
***/
int test1()
{
	cout << "\n ***** Test 1 : instantiation of tree structure of \
			dummy filters + scheduler run ***** \n\n";

	// Create filters (and children filters)
	//detourrage_filter detourf1(0);
	//threshold_filter threshf1(10, (filter*)&detourf1);
	//threshold_filter threshf2(20, (filter*)&detourf1);
	//threshold_filter threshf11(110, (filter*)&threshf1);

	//detourrage_filter detourf2(1);
	//threshold_filter threshf3(11, (filter*)&detourf2);

	//// Enqueue tasks and run the scheduler
	//// Only need to enqueue "root" filters to run all children recursively
	//scheduler<filter> sched;
	//sched.enqueue(&detourf2);
	//sched.enqueue(&detourf1);
	//sched.run();

	return 0;
}


/***
		Test 2 : Run actual blur filters on the image
***/
int test2(int argc, char** argv)
{
	cout << "\n ***** Test 2 : Scheduler run over actual filters ***** \n\n";

	// Load images (reference, background and query)
	Mat ref_raw = getImg(argv[1]);
	Mat back = getImg(argv[2]);
	Mat query_raw = getImg(argv[3]);

	// No third argument then copy ref and pass it as a query image
	if (query_raw.empty())
	{
		cerr << "[WARNING] : No query image passed as a 3rd argument." << endl
			<< "\t Setting query to reference image (default)." << endl;
		query_raw = ref_raw;
	}
	ShowImage(query_raw);

	Mat mask;

	gray_filter gray(ref_raw);
	blur_filter blur0(&gray, 9, 3.5);
	threshold_filter thresh(&blur0,0,255, THRESH_OTSU);
	blur_filter blur1(&blur0, 9, 3.5);
	
	scheduler<filter> sched;
	sched.enqueue(&gray);
	sched.run();

	return 0;
}