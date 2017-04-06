#pragma once
#include "IFilter.h"
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeStatus.h>"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;

namespace filter {
	namespace Algos {
		class InputData;
	}
}
namespace filter
{
	namespace Algos
	{
		struct UserData {
			bool clicked;
			cv::String windowName;
			cv::Mat img;
			std::vector<cv::Point> areaInfos;
		};

		inline void mouse_call(int event, int x, int y, int f, void* _userData){
			auto& userData = *static_cast<UserData *>(_userData);

			switch (event){

			case  CV_EVENT_LBUTTONDOWN:
				if (!userData.clicked){
					userData.clicked = true;
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

		inline cv::Mat objectAreaDrawer(const Mat & imageRef, UserData & userData)
		{
			const char * winName = "AreaDrawer";
			cv::Mat crop;
			cv::namedWindow(winName, cv::WINDOW_NORMAL);


			userData.img = imageRef.clone();
			userData.clicked = false;
			setMouseCallback(winName, mouse_call, &userData);


			while (1) {
				imshow(winName, userData.img);
				auto ret = waitKey(30);

				if (ret == 27) {
					break;
				}
				if (ret == 13) // Return pressed
				{
					std::cout << "Keyboard has been pressed : " << ret << std::endl;
					break;
				}
				if (ret > 0)
				{
					std::cout << "unkonwn Keyboard has been pressed : " << ret << std::endl;
				}
			}
			destroyWindow(winName);
			if (userData.areaInfos.size() < 4)
			{
				std::cout << "Processs has terminated prematuratly" << std::endl;

			}
			else
			{
				std::cout << "Corners are : ";
				for (auto i = 0; i < userData.areaInfos.size(); i++)
				{
					std::cout << "Point : " << userData.areaInfos[i] << std::endl;
				}
				cv::Rect roi;
				roi.x = userData.areaInfos[0].x;
				roi.y = userData.areaInfos[0].y;
				roi.width = userData.areaInfos[1].x - roi.x;
				roi.height = userData.areaInfos[2].y - roi.y;
				std::cout << "width " << roi.width << std::endl;
				std::cout << "height " << roi.height << std::endl;
				std::cout << "img.cols " << userData.img.cols << std::endl;
				std::cout << "img.rows " << userData.img.rows << std::endl;

				crop = imageRef(roi);
			}
			return  crop;
		}

		class Crop : public filter::IFilter
		{
			REGISTER(Crop, ())
			{

			}

			REGISTER_P(float, unused_parameter);
			virtual std::string resultAsString() { return std::string("TODO"); };


		public:

			HipeStatus process(std::shared_ptr<filter::data::IOData> & outputData)
			{
				outputData.reset(new data::OutputData());
				using namespace cv;
				using namespace std;

				cv::Mat img1 = _data.getInputData(0);

				//cv::setMouseCallback("Original", mouse_call, NULL); //setting the mouse callback for selecting the region with mouse
				UserData userData;
				cv::Mat res;
				res = objectAreaDrawer(img1, userData);
				
				if (!res.empty()){
					cv::imwrite("resCrop.png", res);
					outputData.get()->addInputData(res);
					outputData.get()->addInputData(img1);
					return OK;
				}
				return EMPTY_RESULT;
			}
		};
		ADD_CLASS(Crop, unused_parameter);
	}
}