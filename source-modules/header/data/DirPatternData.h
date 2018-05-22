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
#include <coredata/IODataType.h>
#include <coredata/Data.h>
#include <coredata/IOData.h>
#include <data/ImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace data
{
	/**
	* \brief PatternData is the data type used to handle an image, information on its regions of interest (\see SquareCrop), and a request image to find on those regions. Uses OpenCV.
	*/
	 class DATA_EXPORT DirPatternData : public VideoData<DirPatternData>
	{
		Data _inputSource;
		DirectoryImgData dir;


	protected:
		DirPatternData(IOData::_Protection priv) : VideoData(DIRPATTERN)
		{

		}

	public:
		/**
		* \brief Copy constructor for PatternDate copy
		* \param left antoher PatternData
		*/
		DirPatternData(const DirPatternData& left) : VideoData(left.getType())
		{
			Data::registerInstance(left._This);
		}
		DirPatternData() : VideoData<DirPatternData>(IODataType::DIRPATTERN)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));
			ImageData inputImage;
			This()._inputSource = static_cast<Data>(inputImage);
		}


		using VideoData::VideoData;
		DirPatternData(const Data& base) : VideoData(base)
		{
		}

		/**
		* \brief A copy Constructor accepting an image (ImageData). Overwrites the input source image
		* \param inputImage The image used to overrite the input source one
		*/
		DirPatternData(ImageData &inputImage) : VideoData(IODataType::DIRPATTERN)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));

			This()._inputSource = static_cast<Data>(inputImage);
		}

		DirPatternData(Data &inputData, Data &directoryImgData) : VideoData(IODataType::DIRPATTERN)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));

			This()._inputSource = inputData;
			This().dir = static_cast<DirectoryImgData&>(directoryImgData);
		}


		/**
		* \brief Overloaded ssignment operator used to copy PatternData objects.
		* \param left The PatternData object to use as a source for the copy
		* \return Returns a reference to the copied PatternData object
		*/
		virtual DirPatternData& operator=(const DirPatternData& left);
		/**
		* \todo
		* \brief Overloaded insersion operator used to copy PatternData objects.
		* \param left The PatternData object to use as a source for the copy
		* \return Returns a reference to the copied PatternData object
		*/
		DirPatternData& operator<<(const DirPatternData& left);
		/**
		* \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
		* \param left The ImageData used to overwrite The PatternData's one
		* \return Returns a reference to the copied PatternData objet
		*/
		DirPatternData& operator<<(const ImageData& left);

		Data imageSource() const;

		DirectoryImgData DirectoryImg() const;
		
		/**
		* \brief Copy the data of the object to another one
		* \param left The PatternData object to overwrite
		*/
		void copyTo(DirPatternData& left) const;

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
		DirPatternData & operator=(const Data& left);


	};
}

