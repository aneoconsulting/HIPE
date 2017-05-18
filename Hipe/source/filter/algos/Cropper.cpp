#include <filter/Algos/Cropper.h>
#include "Algos/Crop.h"

namespace filter
{
	namespace algos
	{

		void cropper_mouse_call(int event, int x, int y, int f, void* cvCropperData)
		{
			auto& userData = *static_cast<std::shared_ptr<Cropper::CVCropperData> *>(cvCropperData);

			switch (event) {

			case  CV_EVENT_LBUTTONDOWN:
				if (!userData->clicked) {
					userData->clicked = true;
					userData->drawing = true;

					cv::Point pt1;
					pt1.x = x;
					pt1.y = y;

					userData->rectangle.push_back(pt1);
					userData->rectangle.push_back(pt1);
				}

				userData->sandBoxImage.copyTo(userData->sandBoxImage_backup);
				break;
			case CV_EVENT_MOUSEMOVE:
				if (userData->drawing)
				{
					cv::Point pt2;
					pt2.x = x;
					pt2.y = y;
					userData->rectangle[userData->index + 1] = pt2;
					userData->sandBoxImage_backup.copyTo(userData->sandBoxImage);

					for (int i = 0; i < userData->index + 2; i+=2)
					{
						cv::rectangle(userData->sandBoxImage, userData->rectangle[i], userData->rectangle[i + 1], Scalar(255, 0, 0), 4);
					}
				}
				
				break;
			case  CV_EVENT_LBUTTONUP:
				if (userData->clicked) {
					cv::Point pt2;
					pt2.x = x;
					pt2.y = y;
					userData->rectangle[userData->index + 1] = pt2;
					userData->sandBoxImage_backup.copyTo(userData->sandBoxImage);

					for (int i = 0; i < userData->index + 2; i += 2)
					{
						cv::rectangle(userData->sandBoxImage, userData->rectangle[i], userData->rectangle[i + 1], Scalar(255, 0, 0), 4);
					}

					userData->index += 2;

					userData->clicked = false;
					userData->drawing = false;
				}
				break;

			default:
				break;
			}
		}
	
	}
}