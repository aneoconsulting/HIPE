#include <data/PatternData.h>

namespace data
{
	PatternData::PatternData(const std::vector<Data>& left) : VideoData(IODataType::PATTERN), _squareCrop(ImageData(), std::vector<int>()), _endOfSource(-1)
	{
		Data::registerInstance(new PatternData(IOData::_Protection()));
		bool crop_found = false;
		bool source_found = false;

		const int awaitedDataCount = 2;
		if (left.size() != awaitedDataCount)
		{
			std::stringstream errorMessage;
			errorMessage << "Error in PatternData - Incorrect input data count to create a PatternData object. ";
			errorMessage << "Expected " << awaitedDataCount << ", found " << left.size();
			errorMessage << std::endl;

			throw HipeException(errorMessage.str());
		}

		for (auto dataPattern : left)
		{
			if (dataPattern.getType() == SQR_CROP)
			{
				crop_found = true;
				This()._squareCrop = static_cast<const SquareCrop&>(dataPattern);
			}

			else if (isInputSource(dataPattern.getType()))
			{
				source_found = true;
				This()._inputSource = dataPattern;
			}
		}

		if (crop_found == false || source_found == false)
		{
			std::stringstream errorMsg;
			errorMsg << "Some data were not found to build patternData\n";
			errorMsg << "Crop found   : " << (crop_found ? " OK " : "FAIL");
			errorMsg << "Source found : " << (source_found ? " OK " : "FAIL");
			throw HipeException();
		}
	}

	PatternData& PatternData::operator=(const PatternData& left)
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

	PatternData& PatternData::operator<<(const PatternData& left)
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

	PatternData& PatternData::operator<<(const ImageData& left)
	{
		This()._requestImg = left;
		//This()._endOfSource = -1;
		return *this;
	}

	bool PatternData::isVideoSource(IODataType dataType)
	{
		return DataTypeMapper::isStreaming(dataType) || DataTypeMapper::isVideo(dataType);
	}

	bool PatternData::isImageSource(IODataType dataType)
	{
		return DataTypeMapper::isImage(dataType);
	}

	bool PatternData::isInputSource(IODataType dataType)
	{
		return isVideoSource(dataType) || isImageSource(dataType);
	}

	ImageData PatternData::imageRequest() const
	{
		return This_const()._requestImg;
	}

	SquareCrop PatternData::crops() const
	{
		return This_const()._squareCrop;
	}

	std::vector<cv::Mat> PatternData::patterns()
	{
		return This()._squareCrop.crops();
	}

	SquareCrop PatternData::getSquareCrop() const
	{
		return This_const()._squareCrop;
	}

	void PatternData::copyTo(PatternData& left) const
	{
		cv::Mat image;
		This_const().imageRequest().getMat().copyTo(image);
		ImageData imageRequest = image;
		left.This()._requestImg = imageRequest;
		left.This()._inputSource = This_const()._inputSource;
		left.This()._squareCrop = This_const()._squareCrop;
	}

	Data PatternData::newFrame()
	{
		auto sourceType = This_const()._inputSource.getType();
		if (isImageSource(sourceType))
		{
			ImageArrayData& images = static_cast<ImageArrayData &>(This()._inputSource);

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
		else if (isVideoSource(sourceType))
		{
			if (sourceType != VIDF) throw HipeException("Error in PatternData::newFrame - Only video files (VIDF data types) are yet handled as input source.");

			VideoData<FileVideoInput>& video = static_cast<VideoData<FileVideoInput> &>(This()._inputSource);
			Data res = video.newFrame();
			if (isImageSource(res.getType()) == false)
				throw HipeException("Something is going wrong with patternData and Video source ? ");
			This()._requestImg = static_cast<ImageData&>(res);

			return static_cast<Data>(*this);
		}
		return Data();
	}

	bool PatternData::empty() const
	{
		if (This_const()._requestImg.empty()) return true;
		const cv::Mat res = This_const()._requestImg.Array_const()[0];

		return res.empty();
	}

	PatternData& PatternData::operator=(const Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}
}
