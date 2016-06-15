#include "misc.h"
#include "keypoints.h"
#include "ExtractBackground.h"

int main(int argc, char** argv)
{
	cout << "Tests images : BEGIN" << endl;

	// Load images (reference, background and query)
	Mat raw = getImg(argv[1]);
	Mat back = getImg(argv[2]);

	// Test 1 : keypoint search on raw image
	keypoints_SURF(raw);
	keypoints_ORB(raw);
	keypoints_SIFT(raw);
	waitKey(0);

	// Test 2 : background subtraction
	Mat mask;
	processBackgroundFilter(raw, back, mask);
	waitKey(0);

	// Test 3 : Keypoint search on detourred object
	keypoints_SURF(raw);
	keypoints_ORB(raw);
	keypoints_SIFT(raw);
	waitKey(0);

	cout << "Press key" << endl;
	waitKey(0);

	return 0;
}