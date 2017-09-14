#include "drawRect.h"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui.hpp>
#include "tools.h"
#include <iostream>

using namespace cv;

void onMouse(int event, int x, int y, int f, void* _userData){

	UserData &userData = *((UserData *)_userData);



	switch (event){

	case  CV_EVENT_LBUTTONDOWN:
		if (!userData.clicked){

			userData.clicked = true;
			int nbPoint = userData.areaInfos.size();

			
		}
		break;

	case  CV_EVENT_LBUTTONUP:
		if (userData.clicked){

			int nbPoint = userData.areaInfos.size();
			circle(userData.img, Point(x, y), 5, Scalar(0, 0, 255), 4);
			if (nbPoint == 0 || nbPoint >= 4)
			{
				userData.areaInfos.clear();
				userData.areaInfos.push_back(cv::Point(x, y));
			}
			else if (nbPoint == 3)
			{
				userData.areaInfos.push_back(cv::Point(x, y));
				line(userData.img, userData.areaInfos[nbPoint - 1], userData.areaInfos[nbPoint], Scalar(255, 0, 0), 4);
				line(userData.img, userData.areaInfos[0], userData.areaInfos[nbPoint], Scalar(255, 0, 0), 4);
			}
			else if (nbPoint >= 1 && nbPoint < 3)
			{
				userData.areaInfos.push_back(cv::Point(x, y));
				line(userData.img, userData.areaInfos[nbPoint - 1], userData.areaInfos[nbPoint], Scalar(255, 0, 0), 4);
			}
			
			
			
			
			userData.clicked = false;
		}
		break;

	default:
		break;


	}
}



void objectAreaDrawer(const Mat & imageRef, UserData & userData)
{
	
	const char * winName = "AreaDrawer";
	cv::namedWindow(winName, cv::WINDOW_NORMAL);


	userData.img = imageRef.clone();
	userData.clicked = false;
	setMouseCallback(winName, onMouse, &userData);


	while (1) {
		imshow(winName, userData.img);
		int ret = waitKey(30);

		if (ret == 27) {
			break;
		}
		else if (ret == 13) // Return pressed
		{
			std::cout << "Keyboard has been pressed : " << ret << std::endl;
			break;
		}
		else if (ret > 0)
		{
			std::cout << "unkonwn Keyboard has been pressed : " << ret << std::endl;
		}
	}
	destroyWindow(winName);
	if (userData.areaInfos.size() < 4)
	{
		std::cout << "Processs has terminated prematuratly" << std::endl;
			
	} else
	{
		std::cout << "Corners are : ";
		for (int i = 0; i < userData.areaInfos.size(); i++)
		{
			std::cout << "Point : " << userData.areaInfos[i] << std::endl;
		}
	}
	//system("pause");

}



