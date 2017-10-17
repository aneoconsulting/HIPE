#include <data/Composer.h>

namespace data
{
	Data Composer::loadListIoData(const boost::property_tree::ptree& dataNode)
	{
		std::vector<Data> res;

		auto child = dataNode.get_child("array");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			auto iodata = getDataFromComposer(itarray->second);
			res.push_back(iodata);
		}

		return static_cast<Data>(ListIOData(res));

	}

	Data Composer::loadPatternData(const boost::property_tree::ptree& dataNode)
	{
		std::vector<Data> res;
		auto child = dataNode.get_child("desc");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			const std::string dataType = itarray->first;

			auto data = getDataFromComposer(dataType, itarray->second);

			res.push_back(data);
		}
		PatternData pattern(res);

		return static_cast<Data>(pattern);
	}

	Data Composer::loadSquareCrop(const boost::property_tree::ptree& cropTree)
	{
		std::vector<Data> res;
		std::vector<int> pts;

		auto pcitureJson = cropTree.get_child("IMGF");
		Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
		ImageData picture(static_cast<const ImageData &>(data_from_composer));


		if (cropTree.count("crop") != 0)
		{
			pts = as_vector<int>(cropTree, "crop");
		}
		else
		{
			const cv::Mat& cropImage = picture.getMat();

			//Then the image itself is the crop
			pts.push_back(0);
			pts.push_back(0);
			pts.push_back(cropImage.cols);
			pts.push_back(cropImage.rows);
		}
		SquareCrop squareCrop(picture, pts);

		return squareCrop;
	}

	Data Composer::getDataFromComposer(const std::string datatype, const boost::property_tree::ptree& dataNode)
	{
		IODataType ioDataType = DataTypeMapper::getTypeFromString(datatype);
		switch (ioDataType)
		{
		case IODataType::IMGF:
			checkJsonFieldExist(dataNode, "path");
			return loadImageFromFile(dataNode.get<std::string>("path"));
		case IODataType::VIDF:
			checkJsonFieldExist(dataNode, "path");
			return loadVideoFromFile(dataNode);
		case IODataType::SEQIMGD:
			checkJsonFieldExist(dataNode, "path");
			return loadImagesFromDirectory(dataNode.get<std::string>("path"));
		case IODataType::STRMVID:
			checkJsonFieldExist(dataNode, "path");
			return loadVideoFromStream(dataNode.get<std::string>("path"));
		case IODataType::LISTIO:
			checkJsonFieldExist(dataNode, "array");
			return loadListIoData(dataNode);
		case IODataType::PATTERN:
			checkJsonFieldExist(dataNode, "desc");
			return loadPatternData(dataNode);
		case IODataType::SQR_CROP:
			checkJsonFieldExist(dataNode, "IMGF");
			return loadSquareCrop(dataNode);
		case IODataType::IMGB64:
		{
			checkJsonFieldExist(dataNode, "data");
			checkJsonFieldExist(dataNode, "format");
			std::string format = dataNode.get<std::string>("format");
			std::transform(format.begin(), format.end(), format.begin(), ::toupper);

			// width, height, and channels are stored in encoded data
			if (!(format == "JPG" || format == "PNG"))
			{
				checkJsonFieldExist(dataNode, "channels");
				checkJsonFieldExist(dataNode, "width");
				checkJsonFieldExist(dataNode, "height");

				return loadImageFromRawData(dataNode.get<std::string>("data"), dataNode.get<std::string>("format"), dataNode.get<int>("width"), dataNode.get<int>("height"), dataNode.get<int>("channels"));
			}
			return loadImageFromRawData(dataNode.get<std::string>("data"), dataNode.get<std::string>("format"), 0, 0, 0);
		}
		case IODataType::NONE:
		default:
			throw HipeException("Cannot found the data type requested");
		}
	}

	/**
	 * [TODO]
	 * \brief Extract the data from a json tree node (if existing) and load it to its corresponding type
	 * \param dataNode The node to query
	 * \return the loaded data (if existing) in its corresponding type (casted to the type Data)
	 */
	Data Composer::getDataFromComposer(const boost::property_tree::ptree& dataNode)
	{
		checkJsonFieldExist(dataNode, "type");
		auto datatype = dataNode.get<std::string>("type");

		return getDataFromComposer(datatype, dataNode);
	}

	bool Composer::checkJsonFieldExist(const boost::property_tree::ptree& jsonNode, std::string key, bool throwException)
	{
		if (jsonNode.count(key) == 0)
		{
			if (throwException) throw HipeException("Cannot find field json request. Requested field is : " + key);

			return false;
		}

		return true;
	}

	Data Composer::loadImageFromFile(std::string strPath)
	{
		return static_cast<Data>(FileImageData(strPath));
	}

	Data Composer::loadImageFromRawData(const std::string& rawData, const std::string& format, int width, int height, int channels)
	{
		return static_cast<Data>(FileImageData(rawData, format, width, height, channels));
	}

	Data Composer::loadImagesFromDirectory(std::string strPath)
	{
		return static_cast<Data>(DirectoryImgData(strPath));
	}

	Data Composer::loadVideoFromFile(const boost::property_tree::ptree& dataNode)
	{
		std::string path = dataNode.get<std::string>("path");
		bool loop = false;
		if (dataNode.count("loop") != 0)
		{
			loop = dataNode.get<bool>("loop");
		}
		return static_cast<Data>(FileVideoInput(path, loop));
	}
}