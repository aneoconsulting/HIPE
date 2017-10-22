#include <data/IOData.h>
#include <json/JsonTree.h>
#include <Composer.h>

namespace orchestrator
{
	data::Data Composer::loadListIoData(json::JsonTree& dataNode)
	{
		using namespace data;
		std::vector<Data> res;

		auto child = dataNode.allchildren("array");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			auto iodata = getDataFromComposer(*itarray->second);
			res.push_back(iodata);
		}

		return static_cast<Data>(ListIOData(res));
	}

	data::Data orchestrator::Composer::loadPatternData(json::JsonTree& dataNode)
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
