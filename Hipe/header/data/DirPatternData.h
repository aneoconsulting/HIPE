#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <data/IODataType.h>
#include <data/IOData.h>
#include <data/VideoData.h>
#include <data/PatternData.h>

namespace filter
{
	namespace data
	{
		/**
		* \brief PatternData is the data type used to handle an image, information on its regions of interest (\see SquareCrop), and a request image to find on those regions. Uses OpenCV.
		*/
		class DirPatternData : public VideoData<DirPatternData>
		{

		private:
			std::vector<PatternData> dirPatternData;
		



		protected:
			DirPatternData(IOData::_Protection priv) : VideoData(DIRPATTERN)
			{

			}

		public:
			DirPatternData() : VideoData<DirPatternData>(IODataType::DIRPATTERN)
			{
				Data::registerInstance(new DirPatternData(IOData::_Protection()));
	    	}


			using VideoData::VideoData;
			DirPatternData(const Data& base) : VideoData(base)
			{
			}

			

		
			virtual DirPatternData& operator=(const DirPatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left._This);

				return *this;
			}

				DirPatternData& operator<<(const DirPatternData& left)
			{
				if (_This == left._This) return *this;

				Data::registerInstance(left._This);


				return *this;
			}

			
				DirPatternData& operator<<(const PatternData& left)
			{
				This().dirPatternData.push_back(left);
				//This()._endOfSource = -1;
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

			/**
			* \brief Check if the source included in the pattern is a an image
			* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
			* \return true if the source is an image
			*/
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
				return DataTypeMapper::isImage(dataType) || DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isStreaming(dataType);;
			}

			
				std::vector<data::PatternData> patterns()
			{
				return This().dirPatternData;
			}

			
			/**
			* \brief Copy the data of the object to another one
			* \param left The PatternData object to overwrite
			*/
			void copyTo(DirPatternData& left) const
			{
			}

			
	
			inline bool empty() const
			{
				if (This_const().dirPatternData.empty()) return true;
				return false;
			}

			/**
			* [TODO]
			* \brief
			* \param left
			* \return
			*/
			DirPatternData& operator=(const Data& left)
			{
				Data::registerInstance(left);
				_type = left.getType();
				_decorate = left.getDecorate();

				return *this;
			}


		};
	}
}
