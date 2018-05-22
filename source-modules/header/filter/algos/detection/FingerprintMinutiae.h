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

#include <data/ImageArrayData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#pragma warning(pop)

#include <cstdio>
#include <iostream>

namespace filter
{
	namespace algos
	{
		class FingerprintMinutiae : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(FingerprintMinutiae, ()), _connexData(data::INDATA)
			{
				minHessian = 400;
				matchthreshold = 0.1;
				matchcoeff = 2;
				_debug = 0;

			}

			REGISTER_P(int, minHessian);
			REGISTER_P(double, matchthreshold);
			REGISTER_P(double, matchcoeff);

			REGISTER_P(int, _debug);


		public:
			HipeStatus process() override;

			void thinning(cv::Mat &im);
			void thinningIteration(cv::Mat& im, int iter);

			cv::Mat preprocessFingerprint(const cv::Mat& fingerprintImage);
			std::vector<cv::KeyPoint> computeMinutiae(const cv::Mat& fingerprintImage);

			cv::Mat computeMinutiaeDescriptors(const cv::Mat& fingerprintImage, std::vector<cv::KeyPoint>& minutiae);
			std::vector<cv::DMatch> matchFingerprints(const cv::Mat& refFingerprintImage, const cv::Mat& refFingerprintDescriptors, const cv::Mat& queryFingerprintImage, const cv::Mat& queryFingerprintDescriptors, const std::vector<cv::KeyPoint>& refFingerprintKeypoints = std::vector<cv::KeyPoint>(), const std::vector<cv::KeyPoint>& queryFingerprintKeypoints = std::vector<cv::KeyPoint>(), bool parallelOnly = false);

			cv::Mat drawMatches(const std::vector<cv::DMatch>& matches, const cv::Mat& refFingerprintImage, const std::vector<cv::KeyPoint>& refKeypoints, const cv::Mat& queryFingerprintImage, const std::vector<cv::KeyPoint>& queryKeypoints);

			bool areDescriptorsParallel(const std::vector<cv::Point2f>& refDescriptorsPositions, const std::vector<cv::Point2f>& queryDescriptorsPositions, float threshold = 5.0f);

			void debugShow(const std::string& name, const cv::Mat& image, bool deleteWindow = false);

		};

		ADD_CLASS(FingerprintMinutiae, minHessian, matchthreshold, matchcoeff, _debug);
	}
}
#pragma once
