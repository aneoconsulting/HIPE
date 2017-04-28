#pragma once
#include "IOData.h"
#include "IODataType.h"
#include <core/HipeException.h>
#include <boost/property_tree/ptree.hpp>
#include "FileImageData.h"
#include <filter/data/FileVideoInput.h>
#include <filter/data/DirectoryImgData.h>
#include <filter/data/ListIOData.h>
#include <filter/data/StreamVideoInput.h>

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

			static std::shared_ptr<IOData> loadImageFromFile(std::string strPath)
			{
				return std::shared_ptr<IOData>(new FileImageData(strPath));
			}
			static std::shared_ptr<IOData> loadImagesFromDirectory(std::string strPath)
			{
				return std::shared_ptr<IOData>(new DirectoryImgData(strPath));
			}
			static std::shared_ptr<FileVideoInput> loadVideoFromFile(const std::string& path)
			{
				return std::shared_ptr<FileVideoInput>(new FileVideoInput(path));
			}
			static std::shared_ptr<StreamVideoInput> loadStreamVideoFromUrl(const std::string& streamUrl)
			{
				return std::make_shared<StreamVideoInput>(streamUrl);
			}
			//TODO: SZ 
			static std::shared_ptr<ListIOData> loadListIoData(const boost::property_tree::ptree& dataNode)
			{
				std::vector<IOData> res;
				
				auto it = dataNode.begin();
				auto itType = it->second.data();
				if (DataTypeMapper::getTypeFromString(itType) != IODataType::LISTIO)
				{
					throw HipeException("It must be a listio type");
				}
				++it; // get array
				auto array=	it->second;
				for (auto itarray = array.begin(); itarray != array.end(); ++itarray)
				{
					std::cout << itarray->first << "," << itarray->second.data() << std::endl;
					auto iodata = getDataFromComposer(itarray->second);
					res.push_back(*iodata);
				}
							
				auto ret= std::make_shared<ListIOData>(res);
				return ret;
			}
		
			static std::shared_ptr<IOData> getDataFromComposer(const boost::property_tree::ptree& dataNode)
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
					return loadStreamVideoFromUrl(dataNode.get<std::string>("path"));
				case IODataType::LISTIO:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "array");
					ret =  loadListIoData(dataNode);
					return ret;
				case IODataType::NONE:
				default:
					throw HipeException("Cannot found the data type requested");
				}
			}
		};
	}
}
