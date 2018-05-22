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
#include <core/HipeException.h>

#include <coredata/IODataType.h>
#include <coredata/IOData.h>
#include <data/ImageData.h>
#include <data/SquareCrop.h>
#include <data/VideoData.h>
#include <data/FileVideoInput.h>

#include <coredata/data_export.h>

#pragma warning(push, 0) 
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#pragma warning(pop) 


namespace data
{
	/**
	 * \brief PatternData is the data type used to handle an image, information on its regions of interest (\see SquareCrop), and a request image to find on those regions. Uses OpenCV.
	 */
	class DATA_EXPORT PatternData : public VideoData<PatternData>
	{
	public:
		friend  class SquareCrop;

	private:
		SquareCrop _squareCrop;	//<! The list of all the regions of interest. A region on interest is represented by a position (its upper left corner), a width, and a height
		Data _inputSource;		//<! The image where the regions of interest were extracted
		ImageData _requestImg;	//<! The image to compare to the region of interest
		int _endOfSource;		//<! End of video source flag

	protected:
		PatternData(IOData::_Protection priv) : VideoData(PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{

		}

	public:
		PatternData() : VideoData<PatternData>(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(new PatternData(IOData::_Protection()));
			ImageData inputImage;

			This()._inputSource = static_cast<Data>(inputImage);
			newFrame();
		}


		using VideoData::VideoData;
		PatternData(const Data& base) : VideoData(base), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
		}


		/**
		* \brief A copy Constructor accepting an image (ImageData). Overwrites the input source image
		* \param inputImage The image used to overrite the input source one
		*/
		PatternData(ImageData &inputImage) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(new PatternData(IOData::_Protection()));

			This()._inputSource = static_cast<Data>(inputImage);
			newFrame();
		}
			PatternData(ImageData &inputImage, SquareCrop & squareCrope) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
			{
				Data::registerInstance(new PatternData(IOData::_Protection()));

				This()._requestImg = static_cast<Data>(inputImage);
				This()._squareCrop = squareCrope;
				newFrame();
			}



		/**
		 * \brief A copy Constructor accepting a list of 2 Data (CROP and SOURCE)
		 * \param left the list of data
		 */
		PatternData(const std::vector<Data>& left);
				

		/**
		 * \brief Copy constructor for PatternDate copy
		 * \param left antoher PatternData
		 */
		PatternData(const PatternData& left) : VideoData(left.getType()), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
		{
			Data::registerInstance(left._This);

			/*This()._squareCrop = left.This_const()._squareCrop;
			This()._requestImg = left.This_const()._requestImg;
			This()._inputSource = left.This_const()._inputSource;*/
		}


		/**
		 * \brief Overloaded ssignment operator used to copy PatternData objects.
		 * \param left The PatternData object to use as a source for the copy
		 * \return Returns a reference to the copied PatternData object
		 */
		virtual PatternData& operator=(const PatternData& left);

		/**
		 * \todo
		 * \brief Overloaded insersion operator used to copy PatternData objects.
		 * \param left The PatternData object to use as a source for the copy
		 * \return Returns a reference to the copied PatternData object
		 */
		PatternData& operator<<(const PatternData& left);

		/**
		 * \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
		 * \param left The ImageData used to overwrite The PatternData's one
		 * \return Returns a reference to the copied PatternData objet
		 */
		PatternData& operator<<(const ImageData& left);

		/**
		 * \brief Check if the source included in the pattern is a a video coming from a file or coming from a streaming input
		 * Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
		 * \return true if the  source is a video or a streaming video
		 */
		static bool isVideoSource(IODataType dataType);

		/**
		* \brief Check if the source included in the pattern is a an image
		* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
		* \return true if the source is an image
		*/
		static bool isImageSource(IODataType dataType);

		/**
		 * \brief Control if the source is an expected entry
		 * \param dataType
		 * \return
		 */
		static bool isInputSource(IODataType dataType);

		/**
		 * \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
		 * Info : At this step the patternData need to have an ImageData source ONLY.
		 * \return ImageData containing the source to challenge to the pattern image
		 */
		ImageData imageRequest() const;

		/**
		* \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
		* Info : At this step the patternData need to have an ImageData source ONLY.
		* \return ImageData containing the source to challenge the pattern image
		*/
		SquareCrop crops() const;

		/**
		* \brief  The function patterns generate an array of cv::Mat with all the crop representing a sub-matrix of the pattern image.
		* \return ImageData containing the source to challenge the pattern image
		*/
		std::vector<cv::Mat> patterns();

		/**
		 * \brief Get the regions of interest
		 * \return The SquareCrop object containing all the regions of interest
		 */
		SquareCrop getSquareCrop() const;

		/**
		 * \brief Copy the data of the object to another one
		 * \param left The PatternData object to overwrite
		 */
		void copyTo(PatternData& left) const;

		/**
		 * \brief
		 * \return [TODO]
		 */
		Data newFrame();

				
		/**
		 * \brief Does the request image contain data ?
		 * \return Returns true if the request image doesn't contain any data
		 */
		bool empty() const;

		/**
		 * [TODO]
		 * \brief
		 * \param left
		 * \return
		 */
		PatternData& operator=(const Data& left);
	};
}
