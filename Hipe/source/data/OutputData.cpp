#include <data/OutputData.h>

namespace data
{
	std::string OutputData::getResult() const
	{
		return This_const().result;
	}

	void OutputData::setResult(const std::string& result)
	{
		This().result = result;
	}

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

		This().result = left.This_const().result;

		return *this;
	}

	std::string OutputData::resultAsString() const
	{
		return This_const().result;
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

	boost::property_tree::ptree OutputData::resultAsJson()
	{
		boost::property_tree::ptree resultTree;
		boost::property_tree::ptree outputTree;

		// Case where there's no output data to process
		if (!_This)
		{
			outputTree.add<std::string>("info", "NO Data as response");
			resultTree.add_child("DataResult", outputTree);
			return resultTree;
		}
		if (This().input.getType() != IMGF && This().input.getType() != IMGENC)
		{
			outputTree.add<std::string>("ERROR", "Previous filter give wrong data type");
			resultTree.add_child("DataResult", outputTree);
			return resultTree;
		}
		This().result.clear();

		int data_index = 0;

		// For each image output its data in base64
		ImageArrayData& imgdata = static_cast<ImageArrayData &>(This().input);

		for (auto& mat : imgdata.This_const().Array_const())
		{
			// In addition to the base64 data, we add relevent information to the output
			std::stringstream typeKey;
			typeKey << "type_" << data_index;

			std::stringstream dataKey;
			dataKey << "data_" << data_index;

			std::stringstream widthKey;
			widthKey << "width_" << data_index;

			std::stringstream heightKey;
			heightKey << "height_" << data_index;

			std::stringstream channelsKey;
			channelsKey << "channels_" << data_index;

			std::stringstream formatKey;
			formatKey << "format_" << data_index;

			std::string typeValue = DataTypeMapper::getStringFromType(This().getType());

			outputTree.add<std::string>(typeKey.str(), typeValue);

			if (imgdata.getType() == IMGF)
			{
				outputTree.add<std::string>(formatKey.str(), "RAW");
				outputTree.add<int>(widthKey.str(), mat.cols);
				outputTree.add<int>(heightKey.str(), mat.rows);
				outputTree.add<int>(channelsKey.str(), mat.channels());
			}
			else if (imgdata.getType() == IMGENC)
			{
				ImageEncodedData& imgEncData = static_cast<ImageEncodedData&>(This().input);

				outputTree.add<std::string>(formatKey.str(), imgEncData.getCompression());
				outputTree.add<int>(widthKey.str(), imgEncData.getWidth());
				outputTree.add<int>(heightKey.str(), imgEncData.getHeight());
				outputTree.add<int>(channelsKey.str(), imgEncData.getChannelsCount());
			}

			outputTree.add<std::string>(dataKey.str(), mat2str(mat));

			data_index++;
		}

		std::stringstream output;

		resultTree.add_child("DataResult", outputTree);

		return resultTree;
	}

	void OutputData::copyTo(OutputData& left) const
	{
		if (IOData::getType() != left.getType())
			throw HipeException("Cannot left argument in a ImageData");


		left.This().input = This_const().input;
	}
}
