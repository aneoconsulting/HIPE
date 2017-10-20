#include <data/OutputData.h>

namespace data
{
	OutputData& OutputData::operator=(const Data& left)
	{
		if (left.getType() == IODataType::IMGB64)
		{
			IOData::operator=(left);

			return *this;
		}

		if (left.getType() != IODataType::IMGF && left.getType() != IODataType::IMGENC) throw HipeException("[ERROR] OutputData::operator= - data not of type IMGF");

		if (_This)
		{
			_This.reset();
		}

		Data::registerInstance(new OutputData());
		_decorate = true;
		input = left;
		This().input = left;

		return *this;
	}

	OutputData& OutputData::operator=(const OutputData& left)
	{
		IOData::operator=(left);

		

		return *this;
	}

	

	std::string OutputData::mat2str(const cv::Mat& m)
	{
		cv::Mat src;
		if (!m.isContinuous())
		{
			src = m.clone();
		}
		else
		{
			src = m;
		}

		std::vector<uchar> data(src.datastart, src.dataend);

		// Encode
		return base64_encode(data.data(), data.size());
	}

	

	void OutputData::copyTo(OutputData& left) const
	{
		if (IOData::getType() != left.getType())
			throw HipeException("Cannot left argument in a ImageData");


		left.This().input = This_const().input;
	}
}
