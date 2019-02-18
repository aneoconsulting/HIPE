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


#include <algos/show/Cropper.h>

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

					for (int i = 0; i < userData->index + 2; i += 2)
					{
						cv::rectangle(userData->sandBoxImage, userData->rectangle[i], userData->rectangle[i + 1], cv::Scalar(255, 0, 0), 4);
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
						cv::rectangle(userData->sandBoxImage, userData->rectangle[i], userData->rectangle[i + 1], cv::Scalar(255, 0, 0), 4);
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
