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
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <vector>

#include <data/ImageData.h>
#include <data/ImageArrayData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class IPDesc;
		class PPOC : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(PPOC, ()), _connexData(data::INDATA)
			{
				_debug = false;
			}
			REGISTER_P(bool, _debug);

		public:
			HipeStatus process() override;

		private:
			cv::Mat preprocessBackgroundImage(const cv::Mat& image, const cv::Mat& background, cv::Mat& out_mask);
			cv::Mat matchImageToBackground(const cv::Mat & image, const cv::Mat & imageBackground);

			void findObject(const cv::Mat& referenceImage, const cv::Mat& queryImage, const cv::Mat& imageMask);
			bool isObjectPresent(const cv::Mat& queryImage, const IPDesc& poi);
			
			std::vector<std::vector<cv::Point>> findContours(const cv::Mat& imageGrayscale);
			std::vector<IPDesc> findROIs(cv::Mat& out_image, const cv::Mat& imageMask, const std::vector<std::vector<cv::Point>>& contours);
		
			bool isDescriptorNearImageBorder(const IPDesc& descriptor, const cv::Mat& image);
			bool isDescriptorParallel(const std::vector<cv::Point2f>& query, const std::vector<cv::Point2f>& roi);

			bool isOnROI(const std::vector<cv::Point2f>& kptsQuery, const IPDesc & poi);
		};

		ADD_CLASS(PPOC, _debug);

		class IPDesc
		{
		public:
			IPDesc(cv::Mat object, int pointX, int pointY, int pointWidth, int pointHeight)
				: corner_(cv::Rect(pointX, pointY, pointWidth, pointHeight)), descImg_(object)
			{

			}

			inline cv::Point corner() const { return cv::Point(corner_.x, corner_.y); }
			inline cv::Rect rect() const { return corner_; }
			inline int width() const { return corner_.width; }
			inline int height() const { return corner_.height; }
			inline cv::Mat descImg() const { return descImg_; }

		private:
			cv::Rect corner_;
			cv::Mat descImg_;
		};
	}
}
