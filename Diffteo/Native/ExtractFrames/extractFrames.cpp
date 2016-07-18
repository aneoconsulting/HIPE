#include "misc.h"

int main(int argc, char* argv[])
{
	VideoCapture inputVideo(argv[1]);
	if (!inputVideo.isOpened())
	{
		cout << "Could not open reference" << endl;
		return -1;
	}
	cout << "Video found :: " << argv[1] << endl;

	// Acquire and print information about video and frames
	int vid_width = (int)inputVideo.get(CAP_PROP_FRAME_WIDTH);
	int vid_height = (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT);
	int vid_frames = (int)inputVideo.get(CAP_PROP_FRAME_COUNT);
	float vid_fps = (float)inputVideo.get(CAP_PROP_FPS);

	cout << "Frame resolution: (Width x Height) = " << vid_width << " x " << vid_height << endl;
	cout << "Number of frames: " << vid_frames << endl;
	cout << "Frames per second: " << vid_fps << " [" << int(1000/vid_fps) << " ms between 2 frames]" << endl;

	// Set Output directory for images
	string outDir = "OUTPUT/TestPointHebdo/";
	string outFile = "TestPointHebdo_vue1_";
	string outPath = outDir + outFile;

	// Start extracting frames at given time with given timestep 
	// (all in milliseconds)
	int initialTime = 0;
	double rotationTime = 6000;
	double picsPerRotation = 8;
	double timestep = rotationTime / picsPerRotation;

	inputVideo.set(CAP_PROP_POS_MSEC, initialTime);
	int now = initialTime;

	for (int i=0; i<=picsPerRotation; i++)
	{
		// Set window for display (debugging)
		const string window = "input video" + to_string(i+1);
		namedWindow(window, 0);
		resizeWindow(window, int(400 * vid_width / vid_height), 400);
		moveWindow(window, 400 + 10*i, 100 + 10*i);

		// Get frame at specified time
		Mat frame;
		inputVideo >> frame;
		imshow(window, frame); //waitKey(0);
		imwrite(outPath + to_string(i+1) + ".png", frame);

		// Move video to next timepoint
		now = (int)(now + timestep);
		inputVideo.set(CAP_PROP_POS_MSEC, now);

		// DEBUG : Difference between theoretical and effective value of "now"
		// We see that software chooses the closest frame in time (can be before or after theoretical value of "now")
		// cout << "now (requested) = " << now << "\t now (effective) = " << inputVideo.get(CAP_PROP_POS_MSEC) << endl;
		// cout << "\t difference (ms) = " << now - inputVideo.get(CAP_PROP_POS_MSEC) << endl;
	}

	inputVideo.release();
	waitKey(0);
	return 0;
}