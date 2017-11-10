#pragma once
#include <data/IODataType.h>
#include <data/IOData.h>
#include <data/ImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace filter
{
	namespace data
	{
		/**
		* \brief PatternData is the data type used to handle an image, information on its regions of interest (\see SquareCrop), and a request image to find on those regions. Uses OpenCV.
		*/
		class DirPatternData : public VideoData<DirPatternData>
		{
			Data _inputSource;		
			int _endOfSource;		
			std::string _dirPath;
			ImageData _requestImg;
			Data dir;


		protected:
			DirPatternData(IOData::_Protection priv) : VideoData(DIRPATTERN),_endOfSource(-1)
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
			DirPatternData() : VideoData<DirPatternData>(IODataType::DIRPATTERN), _endOfSource(-1), _dirPath("")
			{
				Data::registerInstance(new DirPatternData(IOData::_Protection()));
				ImageData inputImage;
				//DirectoryImgData directory;
				//This().dir = static_cast<Data>(directory);
				This()._inputSource = static_cast<Data>(inputImage);
			//	newFrame();
			}


			using VideoData::VideoData;
			DirPatternData(const Data& base) : VideoData(base),  _endOfSource(-1)
			{
			}

			DirPatternData(const std::vector<Data>& left) : VideoData(IODataType::DIRPATTERN), _endOfSource(-1)
			{
				registerInstance(new DirPatternData(_Protection()));
				auto pathIsDefined = false;
				auto source_found = false;
				if (left.size() != 2)
					throw HipeException("There more data in the vector than needed");
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

				if (pathIsDefined == false && source_found == false)
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
			DirPatternData(ImageData &inputImage) : VideoData(IODataType::DIRPATTERN),  _endOfSource(-1), _dirPath("")
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
			virtual DirPatternData& operator=(const DirPatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left);
				_type = left.getType();
				_decorate = left.getDecorate();

				return *this;
			}

			/**
			* \todo
			* \brief Overloaded insersion operator used to copy PatternData objects.
			* \param left The PatternData object to use as a source for the copy
			* \return Returns a reference to the copied PatternData object
			*/
			DirPatternData& operator<<(const DirPatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left._This);

				return *this;
			}

			/**
			* \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
			* \param left The ImageData used to overwrite The PatternData's one
			* \return Returns a reference to the copied PatternData objet
			*/
			DirPatternData& operator<<(const ImageData& left)
			{
				This()._inputSource = static_cast<Data>(left);
				//This()._endOfSource = -1;
				return *this;
			}

			static inline bool isImageSource(IODataType dataType)
			{
				return DataTypeMapper::isImage(dataType);
			}

			ImageData imageRequest() const
			{
				return This_const()._requestImg;
			}

			Data imageSource() const
			{
				return This_const()._inputSource;
			}

			Data DirectoryImg() const
			{
				return This_const().dir;
			}

			/**
			* \brief Check if the source included in the pattern is a a video coming from a file or coming from a streaming input
			* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
			* \return true if the  source is a video or a streaming video
			*/
			static bool isVideoSource(IODataType dataType)
			{
				return DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isStreaming(dataType);
			}

			static inline bool isDirectory(IODataType dataType)
			{
				return DataTypeMapper::isSequenceDirectory(dataType);
			}

			/**
			* \brief Control if the source is an expected entry
			* \param dataType
			* \return
			*/
			static inline bool isInputSource(IODataType dataType)
			{
				return DataTypeMapper::isImage(dataType) || DataTypeMapper::isStreaming(dataType);
			}

			std::string PathDir() const
			{			
				return This_const()._dirPath;
			}
		
			/**
			* \brief Copy the data of the object to another one
			* \param left The PatternData object to overwrite
			*/
			void copyTo(DirPatternData& left) const
			{
				left.This()._inputSource = This_const()._inputSource;
				left.This()._dirPath = This_const()._dirPath;
			}

			/**
			* \brief
			* \return [TODO]
			*/
			Data newFrame()
			{
				if (isImageSource(This_const()._inputSource.getType()))
				{
					ImageArrayData & images = static_cast<ImageArrayData &>(This()._inputSource);

					if (This_const()._endOfSource <= -1)
						This()._endOfSource = images.Array().size();

					if (This_const()._endOfSource == 0)
					{
						This()._requestImg = (ImageData(cv::Mat::zeros(0, 0, 0)));
						return static_cast<Data>(*this);
					}

					cv::Mat mat = images.Array()[images.Array().size() - This()._endOfSource];

					--(This()._endOfSource);

					This()._requestImg = ImageData(mat);

					return static_cast<Data>(*this);
				}
				else if (isVideoSource(This()._inputSource.getType()))
				{
					VideoData & video = static_cast<VideoData &>(This()._inputSource);
					Data res = video.newFrame();
					if (isImageSource(res.getType()) == false)
						throw HipeException("Something is going wrong with patternData and Video source ? ");
					This()._requestImg = static_cast<ImageData&>(res);

					return static_cast<Data>(*this);
				}
				return Data();
			}

			/**
			* \brief Does the request image contain data ?
			* \return Returns true if the request image doesn't contain any data
			*/
			inline bool empty() const
			{
				if (This_const()._requestImg.empty() && This_const()._dirPath.empty()) return true;

				return false;
			}

			/**
			* [TODO]
			* \brief
			* \param left
			* \return
			*/
			DirPatternData & operator=(const Data& left)
			{
				Data::registerInstance(left);
				_type = left.getType();
				_decorate = left.getDecorate();

				return *this;
			}


		};
	}
}
