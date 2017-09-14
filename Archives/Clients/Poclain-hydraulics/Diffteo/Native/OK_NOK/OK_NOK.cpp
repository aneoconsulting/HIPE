#include "compareROIs.h"
#include "misc.h"

extern Scalar _OK, _NOK, _DOUBT; // defined in compareROIs.cpp

int main(int argc, char* argv[])
{
	float resize_factor = 4;

	Mat ref_raw = getImg(argv[1], resize_factor);
	Mat query_raw = getImg(argv[2], resize_factor);
	string ROIfile = argv[3];

	// Clone of query image to draw the results of the analysis on
	Mat query_output = query_raw.clone();

	//ShowImage(ref_raw); ShowImage(query_raw);

	// Retrieve list of ROI
	vector<int> ROIinfo;
	read_parameterFile(ROIfile, ROIinfo, resize_factor);

	// Compare ROIs specified in the txt file
	for (int i = 0; i < ROIinfo.size(); i += 4)
	{
		int ROI[4] = { ROIinfo[i], ROIinfo[i + 1], ROIinfo[i + 2], ROIinfo[i + 3] };

		// Crop around ROI in ref image
		Rect myROI_ref(ROIinfo[i], ROIinfo[i + 1], ROIinfo[i + 2], ROIinfo[i + 3]);
		Mat ref_crop = ref_raw(myROI_ref);

		// Crop around bigger image slice containing expected ROI
		// (2x height of original ROI)
		Rect myROI_query(0, (ROIinfo[i+1]-0.5*ROIinfo[i+3]), ref_raw.cols, 2*ROIinfo[i + 3]);
		Mat query_crop = query_raw(myROI_query);

		ShowImage(ref_crop); ShowImage(query_crop);

		// SURF comparison of keypoints
		Scalar ok_nok = compareROIs(ref_crop, query_crop, ROI);

		// Draw result of analysis around ROI
		Point topLeft_corner = (ROI[0], ROI[1]);
		Point bottomRight_corner = (ROI[2], ROI[3]);
		rectangle(query_output, myROI_ref, ok_nok, 2);
	}

	// Show overall results
	string window = "Query";
	namedWindow(window, 0);
	resizeWindow(window, int(600 * query_output.cols / query_output.rows), 600);
	moveWindow(window, 50, 50);
	imshow(window, query_output); waitKey(0);

	return 0;
}