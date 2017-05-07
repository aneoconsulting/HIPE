#pragma once
#include "IOData.h"
#include "IODataType.h"
#include <core/HipeException.h>
#include <boost/property_tree/ptree.hpp>
#include "FileImageData.h"
#include <filter/data/FileVideoInput.h>
#include <filter/data/StreamVideoInput.h>
#include <filter/data/DirectoryImgData.h>
#include <filter/data/ListIOData.h>

namespace filter
{
	namespace data
	{
		std::shared_ptr<ListIOData> ret;

		class Composer
		{
		public:
			
			static inline void checkJsonFieldExist(const boost::property_tree::ptree& jsonNode, std::string key)
			{
				if (jsonNode.count(key) == 0)
				{
					throw HipeException("Cannot find field json request. Requested field is : " + key);
				}
			}

			static Data loadImageFromFile(std::string strPath)
			{
				return static_cast<Data>(FileImageData(strPath));
			}

			static Data loadImagesFromDirectory(std::string strPath)
			{
				return static_cast<Data>(DirectoryImgData(strPath));
			}
			static Data loadVideoFromFile(const std::string& path)
			{
				return static_cast<Data>(FileVideoInput(path));
			}

			static Data loadVideoFromStream(const std::string & path)
			{
				return static_cast<Data>(StreamVideoInput(path));
			}
			 
			static Data loadListIoData(const boost::property_tree::ptree& dataNode)
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
		
			static Data getDataFromComposer(const boost::property_tree::ptree& dataNode)
			{
				auto datatype = dataNode.get<std::string>("type");
			
				IODataType ioDataType = DataTypeMapper::getTypeFromString(datatype);
				switch (ioDataType)
				{
				case IODataType::IMGF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImageFromFile(dataNode.get<std::string>("path"));
				case IODataType::VIDF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromFile(dataNode.get<std::string>("path"));
				case IODataType::SEQIMGD:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImagesFromDirectory(dataNode.get<std::string>("path"));
				case IODataType::STRMVID:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromStream(dataNode.get<std::string>("path"));
				case IODataType::LISTIO:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "array");
					return loadListIoData(dataNode);
				case IODataType::NONE:
				default:
					throw HipeException("Cannot found the data type requested");
				}
			}
		};
	}
}
