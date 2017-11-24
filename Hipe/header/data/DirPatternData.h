#pragma once
#include <data/IODataType.h>
#include <data/IOData.h>
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
		int _endOfSource;
		ImageData _requestImg;
		DirectoryImgData dir;


	protected:
		DirPatternData(IOData::_Protection priv) : VideoData(DIRPATTERN), _endOfSource(-1)
		{

		}

	public:
		/**
		* \brief Copy constructor for PatternDate copy
		* \param left antoher PatternData
		*/
		DirPatternData(const DirPatternData& left) : VideoData(left.getType()), _endOfSource(-1)
		{
			Data::registerInstance(left._This);
		}
		DirPatternData() : VideoData<DirPatternData>(IODataType::DIRPATTERN), _endOfSource(-1)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));
			ImageData inputImage;
			//DirectoryImgData directory;
			//This().dir = static_cast<Data>(directory);
			This()._inputSource = static_cast<Data>(inputImage);
			//	newFrame();
		}


		using VideoData::VideoData;
		DirPatternData(const Data& base) : VideoData(base), _endOfSource(-1)
		{
		}

		DirPatternData(const std::vector<Data>& left) : VideoData(IODataType::DIRPATTERN), _endOfSource(-1)
		{
			registerInstance(new DirPatternData(_Protection()));
			auto pathIsDefined = false;
			auto source_found = false;

			const size_t expectedDataCount = 2;
			if (left.size() != expectedDataCount)
			{
				std::stringstream errorMessage;
				errorMessage << "Error in DirPatternData: ";
				errorMessage << (left.size() > 2 ? "Too much " : "Not Enough ");
				errorMessage << "data needed for creation received. ";
				errorMessage << "Expected " << expectedDataCount << " found " << left.size() << "." << std::endl;

				throw HipeException(errorMessage.str());
			}

			for (auto dataPattern : left)
			{

				if (dataPattern.getType() == SEQIMGD)
				{
					pathIsDefined = true;
					This().dir = dataPattern;

					//This()._dirPath = static_cast<const DirectoryImgData&> (dir).DirectoryPath();
				}

				if (dataPattern.getType() == IMGF)
				{
					source_found = true;
					This()._inputSource = dataPattern;
				}
			}

			if (pathIsDefined == false || source_found == false)
			{
				std::stringstream errorMsg;
				errorMsg << "One or two Data aren't not found to build DirpatternData\n";
				errorMsg << "Crop found   : " << (pathIsDefined ? " OK " : "FAIL");
				errorMsg << "Source found : " << (source_found ? " OK " : "FAIL");
				throw HipeException(errorMsg.str());
			}

		}
		/**
		* \brief A copy Constructor accepting an image (ImageData). Overwrites the input source image
		* \param inputImage The image used to overrite the input source one
		*/
		DirPatternData(ImageData &inputImage) : VideoData(IODataType::DIRPATTERN), _endOfSource(-1)
		{
			Data::registerInstance(new DirPatternData(IOData::_Protection()));

			This()._inputSource = static_cast<Data>(inputImage);
			newFrame();
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

		static bool isImageSource(IODataType dataType);

		ImageData imageRequest() const;

		Data imageSource() const;

		DirectoryImgData DirectoryImg() const;
		/**
		* \brief Check if the source included in the pattern is a a video coming from a file or coming from a streaming input
		* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
		* \return true if the  source is a video or a streaming video
		*/
		static bool isVideoSource(IODataType dataType);

		static bool isDirectory(IODataType dataType);

		/**
		* \brief Control if the source is an expected entry
		* \param dataType
		* \return
		*/
		static bool isInputSource(IODataType dataType);



		/**
		* \brief Copy the data of the object to another one
		* \param left The PatternData object to overwrite
		*/
		void copyTo(DirPatternData& left) const;

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
		DirPatternData & operator=(const Data& left);


	};
}

