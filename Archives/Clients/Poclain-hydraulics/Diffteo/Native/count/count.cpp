#include "misc.h"
#include "IPDesc.h"
#include "ShapeDetector.h"
#include "compareROIs.h"

int main(int argc, char* argv[])
{
	double resize_factor = 4.;

	Mat query_raw = getImg(argv[1], resize_factor, false);
	Mat ref_raw = getImg(argv[2], resize_factor, false);
	Mat query_gray = getImg(argv[1], resize_factor, true);
	Mat ref_gray = getImg(argv[2], resize_factor, true);

	if (ref_raw.empty())
		cerr << "Error, ref image empty or not found." << endl;
	if (query_raw.empty())
		cerr << "Error, query image empty or not found." << endl;
	if (ref_raw.empty() || query_raw.empty())
		return -1;

	// Find circles in ref, and potential circle matches in query
	Point2i minmaxRadius = findShapesRef(ref_raw, ref_gray);
	vector<Point2i> centers = findShapesQuery(query_raw, query_gray, minmaxRadius);

	// Loop over ROIs suggested by the centers of circles found in query
	// and try to match with ref
	Mat query_output = query_raw.clone();
	float coef = 1.;
	for (int i = 0; i < centers.size(); i++)
	{
		int topLeft_x = centers[i].x - coef * ref_raw.cols/2;
		int topLeft_y = centers[i].y - coef * ref_raw.rows/2;
		int ROI_width = coef*ref_raw.cols;
		int ROI_height = coef*ref_raw.rows;

		Rect myROI_query = Rect(max(0, topLeft_x), max(0, topLeft_y),
								min(query_raw.cols - topLeft_x, ROI_width),
								min(query_raw.rows - topLeft_y, ROI_height));

		Mat query_crop = query_gray(myROI_query);
		ShowImage(query_crop);

		// SURF comparison of keypoints
		int myROI_query_tab[4] = { myROI_query.x, myROI_query.y, myROI_query.width, myROI_query.height };
		Scalar ok_nok = compareROIs(ref_gray, query_crop, myROI_query_tab, true);

		// Draw result of analysis around ROI
		Point topLeft_corner = (topLeft_x, topLeft_y);
		Point bottomRight_corner = (topLeft_x + ROI_width, topLeft_y + ROI_height);
		rectangle(query_output, myROI_query, ok_nok, 2);
	}

	// Show overall results
	string window = "Output";
	namedWindow(window, 0);
	resizeWindow(window, int(800 * query_output.cols / query_output.rows), 800);
	moveWindow(window, 50, 50);
	imshow(window, query_output); waitKey(0);
	
	return 0;
}