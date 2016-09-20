#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "diffteo.h"
#include <iostream>
using namespace cv;
using namespace std;

bool compareByPos(const Rect& a, const Rect& b)
{
	int ay = (a.y / 2);
	int ax = (a.x / 2);
	int by = (b.y / 2);
	int bx = (b.x / 2);


	if (abs(ay - by) > 10)
		return (ay < by);
	return (ax < bx);
}

vector<Rect> filterTextArea(const Mat& rgb, vector<Rect>& textAreas)
{
	vector<Rect> filteredTextAreas;

	std::sort(textAreas.begin(), textAreas.end(), compareByPos);

	for (int i = 0; i < textAreas.size(); i++)
	{
		//Filter out
		if ((double)(textAreas[i].height / (double)rgb.rows) > 0.25) continue;
		if ((double)(textAreas[i].height / (double)rgb.rows) < 0.06) continue; // area is a line ?? or something else ?

		//Dilate width the right of the rectangle to check if there is more rect 
		//textAreas[i].width += 0.50 * textAreas[i].width;
		filteredTextAreas.push_back(textAreas[i]);
	}
	textAreas.clear();


	for (int i = 0; i < filteredTextAreas.size(); i++)
		textAreas.push_back(filteredTextAreas[i]);
	filteredTextAreas.clear();
	for (int i = 0, j = 0; i < textAreas.size() && j < textAreas.size(); i++)
	{
		//Merge rect
		Mat draw = rgb.clone();


		//rectangle(draw, textAreas[i], Scalar(0, 255, 0), 1);

		//Add Pivot letter first
		filteredTextAreas.push_back(textAreas[i]);
		Rect& rect = filteredTextAreas[filteredTextAreas.size() - 1];

		Point heightCenter = Point(rect.x + rect.width, rect.y + rect.height / 2);
		for (j = i + 1; j < textAreas.size(); j++)
		{
			Rect& nextRect = textAreas[j];

			Point nextHeightCenter = Point(nextRect.x, nextRect.y + nextRect.height / 2);

			//rectangle(draw, textAreas[j], Scalar(0, 0, 255), 1);
			//ShowImageWait(draw);
			// Consider that 4 pixels or under is the same line of character
			if (abs(nextHeightCenter.y - heightCenter.y) <= 4 &&
				nextHeightCenter.x > rect.x && nextHeightCenter.x < rect.x + rect.width + 0.60 * textAreas[j].width)
			{
				draw = rgb.clone();

				rect.x = min(textAreas[j].x, rect.x);
				rect.y = min(textAreas[j].y, rect.y);

				rect.width = textAreas[j].x + textAreas[j].width - rect.x;
				rect.height = max(textAreas[j].height, rect.height);

				//rectangle(draw, textAreas[i], Scalar(255, 0, 0), 1);
			}
			else
			{
				//rectangle(draw, textAreas[j], Scalar(0, 0, 255), 1);
				i = j - 1;
				break;
			}
		}
	}

	Mat draw = rgb.clone();
	const int inc = 10;

	for (int i = 0; i < filteredTextAreas.size(); i++)
	{
		filteredTextAreas[i].x = max(filteredTextAreas[i].x - inc / 2, 0);
		filteredTextAreas[i].y = max(filteredTextAreas[i].y - inc / 2, 0);
		filteredTextAreas[i].width = filteredTextAreas[i].width + inc;
		filteredTextAreas[i].height = filteredTextAreas[i].height + inc;

		filteredTextAreas[i].width = (filteredTextAreas[i].x + filteredTextAreas[i].width) > draw.cols ? filteredTextAreas[i].width - inc : filteredTextAreas[i].width;
		filteredTextAreas[i].height = (filteredTextAreas[i].y + filteredTextAreas[i].height) > draw.rows ? filteredTextAreas[i].height - inc : filteredTextAreas[i].height;

		rectangle(draw, filteredTextAreas[i], Scalar(255, 0, 0), 4);
	}
	//ShowImageWait(draw);

	return filteredTextAreas;
}

vector<Mat> detectTextArea(const cv::Mat& planSrcRaw)
{
	Mat large = planSrcRaw;
	Mat rgb;
	// downsample and use it for processing
	//pyrDown(large, rgb);
	rgb = large.clone();
	Mat small;

	bilateralFilter(rgb, small, 15, 15 * 2, 15 / 2);
	cvtColor(small, small, CV_BGR2GRAY);

	//create a CLAHE object(Arguments are optional).
	Ptr<CLAHE> clahe = createCLAHE(3.0);
	clahe->apply(small, small);
	//ShowImageWait(small);
	//GaussianBlur(small, small, Size(3, 3), 0);
	bitwise_not(small, small);
	//ShowImageWait(small);

	// morphological gradient
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	//ShowImageWait(grad);

	// binarize
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	//ShowImageNoWait(bw);
	// connect horizontally oriented regions
	Mat connected;
	morphKernel = getStructuringElement(MORPH_RECT, Size(3, 1));
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	//ShowImageWait(connected);
	// find contours
	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	std::vector<std::vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(connected.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0));

	/*Mat debug = Mat::zeros(rgb.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
		drawContours(debug, contours, i, Scalar(255, 255, 255));*/
	//ShowImageWait(debug);

	vector<Rect> textAreas;

	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0, 0, 0);
		// fill the contour
		//drawContours(connected, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		//ShowImageNoWait(mask);
		// ratio of non-zero pixels in the filled region
		double r = (double)countNonZero(maskROI) / (rect.width * rect.height);

		if (r > .20 /* assume at least 45% of the area is filled if it contains text */
			&&
			(rect.height > 10 && rect.width > 8) /* constraints on region size */
			/* these two conditions alone are not very robust. better to use something
			like the number of significant peaks in a horizontal projection as a third condition */
		)
		{
			textAreas.push_back(rect);
			//rectangle(rgb, rect, Scalar(0, 255, 0), 2);
		}
		else
		{
			std::cout << "Rejected rectangle : " << rect << endl;

			//rectangle(rgb, rect, Scalar(0, 0, 255), 2);
		}
	}


	vector<Rect> textAreasfiltered = filterTextArea(rgb, textAreas);
	vector<Mat> textAreasImg;

	for (int i = 0; i < textAreasfiltered.size(); i++)
	{
		Mat txtArea = rgb(textAreasfiltered[i]).clone();
		textAreasImg.push_back(txtArea);
		//ShowImageWait(txtArea);
	}

	

	return textAreasImg;
}
