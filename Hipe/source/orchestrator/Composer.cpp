#include <data/IOData.h>
#include <json/JsonTree.h>
#include <Composer.h>

namespace orchestrator
{
	filter::data::Data orchestrator::Composer::loadListIoData(const json::JsonTree& dataNode)
	{
		if (jsonNode.count(key) == 0)
		{
			if (throwException) throw HipeException("Cannot find field json request. Requested field is : " + key);

			return false;
		}

		return true;
	}

	data::Data Composer::loadImageFromFile(std::string strPath)
	{
		return static_cast<data::Data>(data::FileImageData(strPath));
	}

	data::Data Composer::loadImageFromRawData(const std::string& rawData, const std::string& format, int width, int height, int channels)
	{
		return static_cast<data::Data>(data::FileImageData(rawData, format, width, height, channels));
	}

	data::Data Composer::loadImagesFromDirectory(std::string strPath)
	{
		return static_cast<data::Data>(data::DirectoryImgData(strPath));
	}

	data::Data Composer::loadVideoFromFile(json::JsonTree& dataNode)
	{
		std::string path = dataNode.get("path");
		bool loop = false;
		if (dataNode.count("loop") != 0)
		{
			loop = dataNode.getBool("loop");
		}
		return static_cast<data::Data>(data::FileVideoInput(path, loop));
	}

	data::Data Composer::loadVideoFromStream(const std::string& path)
	{
		return static_cast<data::Data>(data::StreamVideoInput(path));
	}

	data::Data Composer::loadListIoData(json::JsonTree& dataNode)
	{
		using namespace data;
		std::vector<Data> res;

		auto child = dataNode .allchildren("array");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			auto iodata = getDataFromComposer(*itarray->second);
			res.push_back(iodata);
		}

		return static_cast<Data>(ListIOData(res));
	}

	filter::data::Data orchestrator::Composer::loadPatternData(const json::JsonTree& dataNode)
	{
		using namespace data;
		std::vector<Data> res;
		auto child = dataNode.allchildren("desc");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			const std::string dataType = itarray->first;
			auto data = orchestrator::Composer::getDataFromComposer(dataType, *itarray->second);
			res.push_back(data);
		}
		data::PatternData pattern(res);

		return static_cast<Data>(pattern);
	}

	data::Data Composer::loadSquareCrop(json::JsonTree& cropTree)
	filter::data::Data Composer::getDataFromComposer(const json::JsonTree& dataNode)
	{
		std::vector<data::Data> res;
		std::vector<int> pts;

		auto pcitureJson = cropTree.get_child("IMGF");
		data::Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
		data::ImageData picture(static_cast<const data::ImageData &>(data_from_composer));


		if (cropTree.count("crop") != 0)
		{
			pts = as_vector<int>(cropTree, "crop");
		}
		else
		{
			const cv::Mat & cropImage = picture.getMat();

			//Then the image itself is the crop
			pts.push_back(0); pts.push_back(0);
			pts.push_back(cropImage.cols); pts.push_back(cropImage.rows);
		}
		data::SquareCrop squareCrop(picture, pts);

		return static_cast<data::Data>(squareCrop);
	}

	data::Data Composer::getDataFromComposer(const std::string datatype, json::JsonTree& dataNode)
	{
		data::IODataType ioDataType = data::DataTypeMapper::getTypeFromString(datatype);
		switch (ioDataType)
		{
		case data::IODataType::IMGF:
			Composer::checkJsonFieldExist(dataNode, "path");
			return loadImageFromFile(dataNode.get("path"));
		case data::IODataType::VIDF:
			Composer::checkJsonFieldExist(dataNode, "path");
			return loadVideoFromFile(dataNode);
		case data::IODataType::SEQIMGD:
			Composer::checkJsonFieldExist(dataNode, "path");
			return loadImagesFromDirectory(dataNode.get("path"));
		case data::IODataType::STRMVID:
			Composer::checkJsonFieldExist(dataNode, "path");
			return loadVideoFromStream(dataNode.get("path"));
		case data::IODataType::LISTIO:
			Composer::checkJsonFieldExist(dataNode, "array");
			return loadListIoData(dataNode);
		case data::IODataType::PATTERN:
			Composer::checkJsonFieldExist(dataNode, "desc");
			return loadPatternData(dataNode);
		case data::IODataType::SQR_CROP:
			Composer::checkJsonFieldExist(dataNode, "IMGF");
			return loadSquareCrop(dataNode);
		case data::IODataType::IMGB64:
		{
			Composer::checkJsonFieldExist(dataNode, "data");
			Composer::checkJsonFieldExist(dataNode, "format");
			std::string format = dataNode.get("format");
			std::transform(format.begin(), format.end(), format.begin(), ::toupper);

			// width, height, and channels are stored in encoded data
			if (!(format == "JPG" || format == "PNG"))
			{
				Composer::checkJsonFieldExist(dataNode, "channels");
				Composer::checkJsonFieldExist(dataNode, "width");
				Composer::checkJsonFieldExist(dataNode, "height");

				return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), dataNode.getInt("width"), dataNode.getInt("height"), dataNode.getInt("channels"));
			}
			return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), 0, 0, 0);
		}
		case data::IODataType::NONE:
		default:
			throw HipeException("Cannot found the data type requested");
		}
	}

	data::Data Composer::getDataFromComposer(json::JsonTree& dataNode)
	{
		using namespace data;
		checkJsonFieldExist(dataNode, "type");
		auto datatype = dataNode.get("type");

		return getDataFromComposer(datatype, dataNode);
	}

}
