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

		DirPatternData(Data &inputData, DirectoryImgData &directoryImgData) : VideoData(IODataType::DIRPATTERN)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));

			This()._inputSource = static_cast<Data>(inputData);
			This().dir = static_cast<Data>(directoryImgData);
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

