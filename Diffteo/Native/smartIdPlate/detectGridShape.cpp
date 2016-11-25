#include "DetectGridShape.h"
#include <opencv2/core.hpp>
#include "Exceptions.h"
#include <iostream>
#include <opencv2/video.hpp>
#include "diffteo.h"

using namespace cv;

#define SQR(a) ((a) * (a))

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
	float denom;

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
		pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

Point nextPointPIVOT(const Mat & input, Point2f pt, MV_DIR dir, Point limit = Point(0, 0))
{
	bool found_begin = false;
	bool found_end = false;
	int begin;
	int end;

	if (dir == MV_DIR::LEFT)
	{
		const uchar* row = input.ptr(pt.y);
		//shift until next white pixel
		/*if (row[(int)pt.x] == 255)
		{
			for (int x = pt.x - 1; x >= 0; x--)
			{
				if (row[x] == 0)
				{
					pt.x = x;
					break;
				}
			}
		}*/

		//Now look for next line by white pixel
		for (int x = pt.x - 1; x >= 0; x--)
		{
			if (found_begin == false && row[x] == 255)
			{
				found_begin = true;
				begin = x;
				continue;
			}

			if (found_begin == true && row[x] == 0)
			{
				found_end = true;
				end = x;
				return Point(max(0, end + 1 + (begin - end) / 2), pt.y);
			}
		}

		if (found_begin == false) return Point(-1, -1);

		return Point(0, pt.y);
	} 
	else if (dir == RIGHT)
	{
		const uchar* row = input.ptr(pt.y);
		//shift until next white pixel
		if (row[(int)pt.x] == 255)
		{
			for (int x = pt.x + 1; x < input.cols; x++)
			{
				if (row[x] == 0)
				{
					pt.x = x;
					break;
				}
			}
		}

		//Now look for next line by white pixel
		for (int x = pt.x + 1; x < input.cols; x++)
		{
			if (found_begin == false && row[x] == 255)
			{
				found_begin = true;
				begin = x;
				continue;
			}

			if (found_begin == true && row[x] == 0)
			{
				found_end = true;
				end = max(0, x - 1);
				return Point(max(0, begin + (end - begin) / 2), pt.y);
			}
		}

		if (found_begin == false) return Point(-1, -1);

		return Point(-1, pt.y);
	}
	else if (dir == UP)
	{
		Mat col = input.col(pt.x).clone();
		Mat tCol = col.t();

		//ShowImageWait(col);

		const uchar * _col = tCol.ptr<unsigned char>(0);
		//Now look for next line by white pixel
		for (int y = pt.y; y >= limit.y; y--)
		{
			if (found_begin == false && _col[y] == 255)
			{
				found_begin = true;
				begin = y;
				continue;
			}

			if (found_begin == true && _col[y] == 0)
			{
				found_end = true;
				end = max(0, y - 1);
				return Point(pt.x, max(0, begin + (end - begin) / 2));
			}
		}
		if (found_begin == false) return Point(-1, -1);

		return Point(pt.y, 0);
	}
	else
	{
		throw new NotYetImplementedException("Next point need to be implemented for all direction");
	}
}

Point shift(Point orig, Point pt2)
{
	int xmin = min(orig.x, pt2.x);
	int xmax = max(orig.x, pt2.x);
	int ymin = (orig.x == xmin) ? orig.y : pt2.y;
	int ymax = (orig.x == xmax) ? orig.y : pt2.y;

	int orig_x = xmax - xmin;
	int orig_y = ymax - ymin;
	return Point(orig_x, orig_y);
}

template<typename T>
Point_<T> reverse(const Point_<T> & pt)
{
	return Point_<T>(pt.y, pt.x);
}

//In polar format
int bestlineStickV(const Mat & input, Point2f orig, Vec2f line, Vec2f & best)
{
	double nbSampleTheta = 100;
	double nbSampleRho = 60;
	double minAngle = CV_PI / 2 - CV_PI / 12; // (- 15 degrees)
	double maxAngle = CV_PI / 2 + CV_PI / 12; // ( + 15 degrees)
	double rho = line[0];
	double strideTheta = (maxAngle - minAngle) / nbSampleTheta;

	

	double strideRho = (rho) / nbSampleRho;
	if (strideRho < 1.0) strideRho = 1.0;

	int sum;
	int max_sum = 0;
	double t_best = 0.0;

	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sin_t = sin(t);
		double cos_t = cos(t);

		for (double r = 0.0; r < rho; r += strideRho)
		{
			int x = min(input.cols - 1, (int)(orig.x + r * cos_t + 0.5));
			int y = min(input.rows - 1, (int)(orig.y + r * sin_t + 0.5));

			const uchar * rows = input.ptr(y);
			if (rows[x] == 255)
			{
				sum++;
			}
		}

		if (max_sum < sum)
		{
			max_sum = sum;
			t_best = t;
		}
	}
	best = Vec2f(rho, t_best);

	return max_sum;
}

//In polar format
int bestlineStickH(const Mat & input, Point2f orig, Vec2f line_vec, Vec2f & best)
{
	double nbSampleTheta = 60;
	double nbSampleRho = 60;
	double minAngle = - CV_PI / 12; // (- 15 degrees)
	double maxAngle = CV_PI / 12; // ( + 15 degrees)
	double rho = line_vec[0];
	double strideTheta = (maxAngle - minAngle) / nbSampleTheta;
	double strideRho = (rho) / nbSampleRho;

	int sum = 0;
	int max_value = 0;
	double t_best = 0.0;

	Mat search;
	cvtColor(input, search, CV_GRAY2RGB);

	for (double t = maxAngle; t > minAngle; t -= strideTheta)
	{
		sum = 0;
		double sin_t = sin(t);
		double cos_t = cos(t);

		for (double r = 1; r < rho; r += strideRho)
		{
			int x = max(0.0, min((double)input.cols - 1, orig.x + r * cos_t));
			
			int y = max(0.0, min((double)input.rows - 1, orig.y + r * sin_t));


			const uchar * rows = input.ptr(y);
			if (rows[x] == 255)
			{
				sum++;
			}
		}
	/*	line(search, orig, Point2f(orig.x + rho * cos_t, orig.y + rho * sin_t), Scalar(0, 255, 0), 1);
		ShowImageWait(search);*/

		if (max_value < sum)
		{
			max_value = sum;
			t_best = t;
		}
	}
	best = Vec2f(rho, t_best);

	return max_value;
}



void findLineAlignmentLeft(const cv::Mat & input, std::vector<cv::Point2f> & corners)
{
	//TODO do it for all line

	//Begine by the left line
	Point2f pt1 = corners[0];
	Point2f pt2 = corners[3];
	Point2f ptNext;
	MV_DIR dir = MV_DIR::LEFT;

	//get 3 points from this line
	int xmin = min(pt1.x, pt2.x);
	int xmax = max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// be sure the point is ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// move a bit the y to stick on vertical line
	pt1.y = min(input.rows - 1, (int)(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = max(0, (int)(pt2.y - abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	Mat searchLine = input.clone();
	cvtColor(input, searchLine, CV_GRAY2RGB);
	line(searchLine, Point(0, pt1.y), pt1, Scalar(0, 0, 255), 1);
	line(searchLine, Point(0, pt2.y), pt2, Scalar(0, 0, 255), 1);
	//ShowImageWait(searchLine);


	ptNext.x = pt1.x;
	ptNext.y = pt1.y;
	int max = 0;
	Point cartesian_max;
	cartesian_max.x = -1;
	cartesian_max.y = -1;
	double rho = sqrt(SQR(pt2.x - pt1.x) + SQR(pt2.y - pt1.y));

	while (ptNext.x > 0)
	{
		ptNext = nextPointPIVOT(input, ptNext, LEFT);
		//After avoid to shift on a horizontal line a bad rows, we set to the orignal value of row
		
		if (ptNext.x == -1) break;

		

		int xp = xmax - ptNext.x;
		int yp = ymax - ptNext.y;

		double theta = atan2(xp, yp);

		Vec2f best_align;
		

		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		int current_best = bestlineStickV(input, ptNext, Vec2f(rho, theta), best_align);

		if (max < current_best)
		{
			max = current_best;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesian_max.x = ptNext.x + rho * cos(best_align[1]);
			cartesian_max.y = ptNext.y +rho * sin(best_align[1]);
		}
	}

	Mat lineLeft;
	cvtColor(input, lineLeft, CV_GRAY2RGB);
		//	std::cout << "min angle : " << minAngle << std::endl;
		//std::cout << "actual angle : " << theta << std::endl;
		//std::cout << "max angle : " << maxAngle << std::endl;

	

	line(lineLeft, pt1, cartesian_max, Scalar(0, 0, 255), 2);
	//ShowImageWait(lineLeft);

	//Now compute corner and the new intersection
	Vec4i line1, line2;
	line1[0] = corners[0].x; 
	line1[1] = corners[0].y; 
	line1[2] = corners[1].x; 
	line1[3] = corners[1].y; 

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesian_max.x;
	line2[3] = cartesian_max.y;
	corners[0] = computeIntersect(line1, line2);

	line1[0] = corners[2].x;
	line1[1] = corners[2].y;
	line1[2] = corners[3].x;
	line1[3] = corners[3].y;
	corners[3] = computeIntersect(line1, line2);

	return;
}


void findLineAlignmentRight(const cv::Mat & input, std::vector<cv::Point2f> & corners)
{
	//TODO do it for all line

	//Begine by the left line
	Point2f pt1 = corners[1];
	Point2f pt2 = corners[2];
	Point2f ptNext;
	MV_DIR dir = MV_DIR::RIGHT;

	//get 3 points from this line
	int xmin = min(pt1.x, pt2.x);
	int xmax = max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// be sure the point is ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// move a bit the y to stick on vertical line
	pt1.y = min(input.rows - 1, (int)(pt1.y + abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	pt2.y = max(0, (int)(pt2.y - abs(ymax - ymin) / 6.0)); // 6 is the middle of 3 lines of text
	ptNext.x = pt1.x;
	ptNext.y = pt1.y;
	int max = 0;
	Point cartesian_max;
	cartesian_max.x = -1;
	cartesian_max.y = -1;
	double rho = sqrt(SQR(xmax - xmin) + SQR(ymax - ymin));

	while (ptNext.x < input.cols)
	{
		ptNext = nextPointPIVOT(input, ptNext, dir);
		//After avoid to shift on a horizontal line a bad rows, we set to the orignal value of row

		if (ptNext.x == -1) break;
		

		double theta = 0.0;

		Vec2f best_align;


		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		int current_best = bestlineStickV(input, ptNext, Vec2f(rho, theta), best_align);

		if (max < current_best)
		{
			max = current_best;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesian_max.x = ptNext.x + rho * cos(best_align[1]);
			cartesian_max.y = ptNext.y + rho * sin(best_align[1]);
		}
	}

	Mat lineRight;
	cvtColor(input, lineRight, CV_GRAY2RGB);
	//	std::cout << "min angle : " << minAngle << std::endl;
	//std::cout << "actual angle : " << theta << std::endl;
	//std::cout << "max angle : " << maxAngle << std::endl;


	line(lineRight, pt1, cartesian_max, Scalar(0, 0, 255), 1);
	//ShowImageWait(lineRight);

	//Now compute corner and the new intersection
	Vec4i line1, line2;
	line1[0] = corners[0].x;
	line1[1] = corners[0].y;
	line1[2] = corners[1].x;
	line1[3] = corners[1].y;

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesian_max.x;
	line2[3] = cartesian_max.y;
	corners[1] = computeIntersect(line1, line2);

	line1[0] = corners[2].x;
	line1[1] = corners[2].y;
	line1[2] = corners[3].x;
	line1[3] = corners[3].y;
	corners[2] = computeIntersect(line1, line2);

	return;
}


void findLineAlignmentTop(const cv::Mat & input, std::vector<cv::Point2f> & corners)
{
	//TODO do it for all line

	//Begine by the left line
	Point2f pt1 = corners[0];
	Point2f pt2 = corners[1];
	Point2f ptNext;
	MV_DIR dir = MV_DIR::UP;

	//get 3 points from this line
	int xmin = min(pt1.x, pt2.x);
	int xmax = max(pt1.x, pt2.x);
	int ymin = (pt1.x == xmin) ? pt1.y : pt2.y;
	int ymax = (pt2.x == xmax) ? pt2.y : pt1.y;

	// be sure the point is ordered
	pt1.x = xmin; pt2.y = ymin;
	pt2.x = xmax; pt2.y = ymax;

	// move a bit the y to stick on vertical line
	pt1.x = min(input.cols - 1, (int)(pt1.x + abs(xmax - xmin) / 4.0)); // 4 is 1/4 of a horizontal line
	pt2.x = max(0, (int)(pt2.x /*- abs(xmax - xmin) / 4.0*/)); 
	Mat searchLine = input.clone();
	cvtColor(input, searchLine, CV_GRAY2RGB);
	line(searchLine, Point(pt1.x, 0), pt1, Scalar(0, 0, 255), 1);
	line(searchLine, Point(pt2.x, 0), pt2, Scalar(0, 0, 255), 1);
	//ShowImageWait(searchLine);

	ptNext.x = pt1.x;
	ptNext.y = pt1.y;
	int max_value = 0;
	Point cartesian_max;
	cartesian_max.x = -1;
	cartesian_max.y = -1;
	double rho = sqrt(SQR(pt2.x - pt1.x) + SQR(pt2.y - pt2.y));

	Point2f limit; 

	limit.y = (max(0.0, (double)(pt1.y - (double)0.05 * input.rows)));
	limit.x = pt1.x; // ignore the limit on x for top line research

	while (ptNext.y >= 0)
	{
		Point2f point = ptNext = nextPointPIVOT(input, ptNext, dir, limit);

		if (ptNext.x == -1) break;


		double theta = 0.0;

		Vec2f best_align;


		//ptNext.y is somewhere on the vertical line. We want the proper value of y
		int current_best = bestlineStickH(input, ptNext, Vec2f(rho, theta), best_align);

		if (max_value < current_best)
		{
			max_value = current_best;
			pt1.x = ptNext.x; pt1.y = ptNext.y;
			cartesian_max.x = ptNext.x + rho * cos(best_align[1]);
			cartesian_max.y = ptNext.y + rho * sin(best_align[1]);
		}
	}

	Mat lineTop;
	cvtColor(input, lineTop, CV_GRAY2RGB);
	//	std::cout << "min angle : " << minAngle << std::endl;
	//std::cout << "actual angle : " << theta << std::endl;
	//std::cout << "max angle : " << maxAngle << std::endl;


	line(lineTop, pt1, cartesian_max, Scalar(0, 0, 255), 1);
	//ShowImageWait(lineTop);

	//Now compute corner and the new intersection
	Vec4i line1, line2;
	line1[0] = corners[0].x;
	line1[1] = corners[0].y;
	line1[2] = corners[3].x;
	line1[3] = corners[3].y;

	line2[0] = pt1.x;
	line2[1] = pt1.y;
	line2[2] = cartesian_max.x;
	line2[3] = cartesian_max.y;
	corners[0] = computeIntersect(line1, line2);

	line1[0] = corners[1].x;
	line1[1] = corners[1].y;
	line1[2] = corners[2].x;
	line1[3] = corners[2].y;
	corners[1] = computeIntersect(line1, line2);

	return;
}