#pragma once
#include <opencv2/core/types.hpp>
#include "opencv2/highgui.hpp"



#define ShowImageWait(img)\
	imshow(#img, img);\
	waitKey(0);\



#define ShowImageNoWait(img)\
	imshow(#img, img);
