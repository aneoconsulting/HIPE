#include <DirPatternData.h>


#pragma once
#include <data/IODataType.h>
#include <data/IOData.h>
#include <data/ImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace data
{



	/**
	* \brief Overloaded ssignment operator used to copy PatternData objects.
	* \param left The PatternData object to use as a source for the copy
	* \return Returns a reference to the copied PatternData object
	*/
	DirPatternData& DirPatternData::operator=(const DirPatternData& left)
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
	DirPatternData& DirPatternData::operator<<(const DirPatternData& left)
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
	DirPatternData& DirPatternData::operator<<(const ImageData& left)
	{
		This()._inputSource = static_cast<Data>(left);
		//This()._endOfSource = -1;
		return *this;
	}

	bool DirPatternData::isImageSource(IODataType dataType)
	{
		return DataTypeMapper::isImage(dataType);
	}

	ImageData DirPatternData::imageRequest() const
	{
		return This_const()._requestImg;
	}

	Data DirPatternData::imageSource() const
	{
		return This_const()._inputSource;
	}

	DirectoryImgData DirPatternData::DirectoryImg() const
	{
		return This_const().dir;
	}

	/**
	* \brief Check if the source included in the pattern is a a video coming from a file or coming from a streaming input
	* Info : This code will check if the data need a transformation or not before rootfilter push in the Orchestrator
	* \return true if the  source is a video or a streaming video
	*/
	bool DirPatternData::isVideoSource(IODataType dataType)
	{
		return DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isStreaming(dataType);
	}

	bool DirPatternData::isDirectory(IODataType dataType)
	{
		return DataTypeMapper::isSequenceDirectory(dataType);
	}

	/**
	* \brief Control if the source is an expected entry
	* \param dataType
	* \return
	*/
	bool DirPatternData::isInputSource(IODataType dataType)
	{
		return DataTypeMapper::isImage(dataType) || DataTypeMapper::isStreaming(dataType);
	}



	/**
	* \brief Copy the data of the object to another one
	* \param left The PatternData object to overwrite
	*/
	void DirPatternData::copyTo(DirPatternData& left) const
	{
		left.This()._inputSource = This_const()._inputSource;
		left.This().dir = This_const().dir;
	}

	/**
	* \brief
	* \return [TODO]
	*/
	Data DirPatternData::newFrame()
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
	bool DirPatternData::empty() const
	{
		if (This_const()._requestImg.empty() && This_const().dir.empty()) return true;

		return false;
	}

	/**
	* [TODO]
	* \brief
	* \param left
	* \return
	*/
	DirPatternData & DirPatternData::operator=(const Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}
}

