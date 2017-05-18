#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <filter/data/IODataType.h>
#include <filter/data/IOData.h>
#include <filter/data/ListIOData.h>
#include <filter/data/ImageData.h>
#include <filter/data/SquareCrop.h>
#include <filter/data/VideoData.h>

namespace filter
{
	namespace data
	{
		class PatternData : public VideoData<PatternData>
		{
		public:
			friend  class SqaureCrop;

		private:
			SquareCrop _squareCrop;
			Data _inputSource;
			
			ImageData _requestImg;

			int _endOfSource;

		

		
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

		
			/**
			* \brief A copy Constructor accepting a list of 2 Data (CROP and SOURCE)
			* \param left the list of data
			*/
			PatternData(ImageData &inputImage) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
			{
				Data::registerInstance(new PatternData(IOData::_Protection()));

				This()._inputSource = static_cast<Data>(inputImage);
				newFrame();
			}


			/**
			 * \brief A copy Constructor accepting a list of 2 Data (CROP and SOURCE)
			 * \param left the list of data
			 */
			PatternData(const std::vector<Data>& left) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
			{
				Data::registerInstance(new PatternData(IOData::_Protection()));
				bool crop_found = false;
				bool source_found = false;

				if (left.size() != 2)
					throw HipeException("There more data in the vector than needed");
				for (auto dataPattern : left)
				{
					if (dataPattern.getType() == SQR_CROP)
					{
						crop_found = true;
						This()._squareCrop = static_cast<const SquareCrop&>(dataPattern);
					}

					if (isInputSource(dataPattern.getType()))
					{
						source_found = true;
						This()._inputSource = dataPattern;
					}
				}

				if (crop_found == false || source_found == false)
				{
					std::stringstream errorMsg;
					errorMsg << "One or two Data aren't not found to build patternData\n";
					errorMsg << "Crop found   : " << (crop_found ? " OK " : "FAIL");
					errorMsg << "Source found : " << (source_found ? " OK " : "FAIL");
					throw HipeException();
				}
				
			}
		

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
			

			virtual PatternData& operator=(const PatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left._This);

				/*This()._type = left.This_const().getType();
				
				This()._squareCrop = left.This_const()._squareCrop;
				This()._requestImg = left.This_const()._requestImg;
				This()._inputSource = left.This_const()._inputSource;
				This()._endOfSource = left.This_const()._endOfSource;*/


				return *this;
			}

			PatternData& operator<<(const PatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left._This);

				/*This()._type = left.This_const().getType();

				This()._squareCrop = left.This_const()._squareCrop;
				This()._requestImg = left.This_const()._requestImg;
				This()._inputSource = left.This_const()._inputSource;
				This()._endOfSource = left.This_const()._endOfSource;*/

				return *this;
			}

			PatternData& operator<<(const ImageData& left)
			{
				This()._requestImg = left;
				This()._endOfSource = -1;
				return *this;
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

			static inline bool isImageSource(IODataType dataType)
			{
				return DataTypeMapper::isImage(dataType);
			}

			/**
			 * \brief Control if the source is an expected entry
			 * \param dataType 
			 * \return 
			 */
			static inline bool isInputSource(IODataType dataType)
			{
				return DataTypeMapper::isImage(dataType) || DataTypeMapper::isStreaming(dataType) ||  DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isStreaming(dataType);;
			}

			/**
			 * \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
			 * Info : At this step the patternData need to have an ImageData source ONLY.
			 * \return ImageData containing the source to challenge to the pattern image
			 */
			ImageData imageRequest() const
			{
				return This_const()._requestImg;
			}

			/**
			* \brief Return the request image to compare to the pattern. This is the image where the pattern need to be found.
			* Info : At this step the patternData need to have an ImageData source ONLY.
			* \return ImageData containing the source to challenge the pattern image
			*/
			inline SquareCrop crops() const
			{
				return This_const()._squareCrop;
			}

			/**
			* \brief  The function patterns generate an array of cv::Mat with all the crop representing a sub-matrix of the pattern image.
			* \return ImageData containing the source to challenge the pattern image
			*/
			std::vector<cv::Mat> patterns()
			{
				return This()._squareCrop.crops();
			}

			SquareCrop getSquareCrop() const
			{
				return This_const()._squareCrop;
			}

			void copyTo(PatternData& left) const
			{
				cv::Mat image;
				This_const().imageRequest().getMat().copyTo(image);
				ImageData imageRequest = image;
				left.This()._requestImg = imageRequest;
				left.This()._inputSource = This_const()._inputSource;
				left.This()._squareCrop = This_const()._squareCrop;
				
				
			}

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
			}

			inline bool empty() const
			{
				if (This_const()._requestImg.empty()) return true;
				const cv::Mat res = This_const()._requestImg.Array_const()[0];

				return res.empty();
			}

		};
	}
}
