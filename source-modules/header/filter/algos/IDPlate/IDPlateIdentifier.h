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
		/**
		 * \brief The IDPlateIdentifier filter will handle the detection of the character of an ID plate then their recognition with machine learning
		 *
		 * \var IDPlateIdentifier::minXPos
		 * The minimum position on the X Axis to start searching for characters.
		 *
		 * \var IDPlateIdentifier::maxXPos
		 * The maximum position on the X Axis to stop searching for characters.
		 *
		 * \var IDPlateIdentifier::minLines
		 * The minimum number of lines of text we can fit on the image(minimum number of lines means big character).Used to compute the minimum and maximum height a character can have.
		 *
		 * \var IDPlateIdentifier::ratioY
		 * The percentage of the average characters' height. Every characters are not exactly centered on the same line. We use the ratio to search around this line.
		 *
		 * \var IDPlateIdentifier::ratioMinArea
		 * The percentage of the average characters' area. Used to compute the minimum area a character can have to be valid.
		 *
		 * \var IDPlateIdentifier::ratioMaxArea
		 * The percentage of the average characters' area. Used to compute the maximum area a character can have to be valid.
		 *
		 * \var IDPlateIdentifier::_debug
		 * The debug level to use to print informations and show images
		 */
		class IDPlateIdentifier : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(IDPlateIdentifier, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				minXPos = 0.05;
				maxXPos = 0.8;
				minLines = 4;
				ratioY = 0.2;
				ratioMinArea = 0.5;
				ratioMaxArea = 3.0;
				fontScale = 3;
				fontThickness = 3;
			}

			REGISTER_P(int, _debug);
			REGISTER_P(double, minXPos);
			REGISTER_P(double, maxXPos);
			REGISTER_P(int, minLines);
			REGISTER_P(double, ratioY);
			REGISTER_P(double, ratioMinArea);
			REGISTER_P(double, ratioMaxArea);
			REGISTER_P(int, fontScale);
			REGISTER_P(int, fontThickness);

			HipeStatus process() override;

		private:
			/**
			 * \brief Preprocess an ID plate image to help the search of its characters contours.
			 * \param plateImage The color image of the plate
			 * \return Returns a preprocessed binary image where the characters are blackon a white background
			 */
			cv::Mat preprocessImage(const cv::Mat& plateImage);

			/**
			 * \brief A Wrapper function for cropping an image around a ROI
			 * \param image The image on which the ROI will be extracted
			 * \param roi The ROI
			 * \return A new image containing only the cropped rect from the source image
			 */
			cv::Mat cropROI(const cv::Mat& image, const cv::Rect& roi);

			/**
			 * \brief Batch process version of cropROI. Crops an image around multiple ROIs
			 * \param image The image on which the ROI will be extracted
			 * \param rois The ROIs
			 * \return A container filled with all the new images containing only the cropped rects from the source image
			 */
			std::vector<cv::Mat> cropROIs(const cv::Mat& image, const std::vector<cv::Rect>& rois);

			/**
			 * \brief From an image containing characters, generate another one with printed charactersLabels text at charactersRects positions.
			 * \param plateImage The source image to use
			 * \param charactersRects The positions of the rects (where to put text) on the image
			 * \param charactersLabels The text to put for each rect
			 * \return An image where the labels in charactersLabels are drawn in their respective charactersRects position
			 */
			cv::Mat createOutputImage(const cv::Mat& plateImage, const std::vector<cv::Rect>& charactersRects, const std::vector<std::string>& charactersLabels);
		};

		ADD_CLASS(IDPlateIdentifier, _debug, minXPos, maxXPos, minLines, ratioY, ratioMinArea, ratioMaxArea, fontScale, fontThickness);

		class LabelOCR
		{
		public:
			LabelOCR();
			LabelOCR(int debugLevel);
			virtual ~LabelOCR();

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
