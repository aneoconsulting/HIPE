#include "misc.h"
#include "IPdesc.h"

/*** Detect if we're too close to the border by counting pure black pixels **/
bool isBorderdetection(const IPDesc& desc, const Mat& mat);

/*** Return areas of interests (opt : draw circle around each of them) ***/
vector<IPDesc> drawShape(cv::Mat img, cv::Mat & mask, vector<vector<Point>> &contours);