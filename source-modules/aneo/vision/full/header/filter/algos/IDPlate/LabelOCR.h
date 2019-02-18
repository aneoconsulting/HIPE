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
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#include <vector>
#include <string>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/text.hpp>
#pragma warning(pop)


#include "filter/algos/IDPlate/IDPlateTools.h"


namespace filter
{
	namespace algos
	{
		class LabelOCR2
		{
		public:
			LabelOCR2();
			LabelOCR2(int debugLevel);
			virtual ~LabelOCR2();

			void preProcess(const cv::Mat& InputImage, cv::Mat& binImage, int debugLevel = 0);
			std::vector<std::string> runRecognition(const std::vector<cv::Mat>& labelsImages, int labelType);

		private:
			void init();

			std::string runPrediction(const cv::Mat& labelImage, int minConfidence, int imageIndex = -1);

			cv::Mat quantizeImage(const cv::Mat& image, int clusters, int maxIterations = 10);
			cv::Mat binarizeImage(const cv::Mat& image);

			cv::Mat enlargeCharacter(const cv::Mat& character, int margin);

		private:
			cv::Ptr<cv::text::OCRTesseract> tesseOCR_;
			int _debug;
		};
	}
}
