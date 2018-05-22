//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <corefilter/IFilter.h>
#include <corefilter/tools/RegisterClass.h>
#include <data/ImageData.h>
#include <data/ImageArrayData.h>
#include <core/HipeStatus.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;

namespace filter
{
	namespace algos
	{
		/**
		 * \todo
		 * \brief The UserData struct stores the data relative to the user mouse click input
		 */
		struct UserData {
			bool clicked;						//<! Does the user already clicked on the window?
			cv::String windowName;				//<! The name of the window on which the user clicked
			cv::Mat img;						//<! The data of the image (the one in the window) on which the user clicked
			std::vector<cv::Point> areaInfos;	//<! The area the user selected by clickng and dragging the mouse
		};

		/**
		 * \todo
		 * \brief Use this method as a mouse callback to know where the user clicked and define the area he selected.
		 * \param event The type of the mouse event that occured
		 * \param x The x position (relative to the targeted window) where the user clicked.
		 * \param y The y position (relative to the targeted window) where the user clicked.
		 * \param f [TODO]
		 * \param _userData The data relative to where the user clicked.
		 */
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

		/**
		 * \todo
		 * \brief Handles the user mouse input on an OpenCV window, and the area (region of interest) he selects
		 * \param imageRef The image on which the user will select areas
		 * \param userData The structure where the user input data will be stored. \see UserData
		 * \return The extracted image data of the region of interest the user selected.
		 */
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
				for (unsigned int i = 0; i < userData.areaInfos.size(); i++)
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

		/**
		 * \brief The Crop filter will let the user manually select (with mouse input) a region of interest in an image.
		 * 
		 *  The filter will await an image as input and will output a list of regions of interest.
		 *  Prefer using the Cropper filter and the PatternData data type.
		 */
		class Crop : public filter::IFilter
		{
			//data::ConnexData<data::ImageData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageData, data::ImageArrayData);

			REGISTER(Crop, ()) , _connexData(data::WayData::INDATA)
			{
				//_connexData = data::ConnexData<data::ImageData, data::ImageArrayData>(data::WayData::INDATA);
			}

			REGISTER_P(float, unused_parameter);
			virtual std::string resultAsString() { return std::string("TODO"); };


		public:

			HipeStatus process()
			{
				
				using namespace cv;
				using namespace std;

				cv::Mat img1 = _connexData.pop().getMat();

				//cv::setMouseCallback("Original", mouse_call, NULL); //setting the mouse callback for selecting the region with mouse
				UserData userData;
				cv::Mat res;
				res = objectAreaDrawer(img1, userData);
				
				if (!res.empty()){
					cv::imwrite("resCrop.png", res);
					data::ImageArrayData output;
					output << res << img1;
					
					return OK;
				}
				return EMPTY_RESULT;
			}
		};
		ADD_CLASS(Crop, unused_parameter);
	}
}
