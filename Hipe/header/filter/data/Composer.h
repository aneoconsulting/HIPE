#pragma once

#include <boost/property_tree/ptree.hpp>

#include <core/HipeException.h>

#include <filter/data/IOData.h>
#include <filter/data/IODataType.h>

#include <filter/data/FileImageData.h>
#include <filter/data/FileVideoInput.h>
#include <filter/data/StreamVideoInput.h>
#include <filter/data/DirectoryImgData.h>
#include <filter/data/ListIOData.h>
#include <filter/data/SquareCrop.h>
#include <filter/data/PatternData.h>

namespace filter
{
	namespace data
	{
		std::shared_ptr<ListIOData> ret;

		class Composer
		{
		public:
			template <typename T>
			static std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
			{
				std::vector<T> r;
				for (auto& item : pt.get_child(key))
					r.push_back(item.second.get_value<T>());
				return r;
			}

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

			static Data loadPatternData(const boost::property_tree::ptree& dataNode)
			{
				std::vector<Data> res;
				auto child = dataNode.get_child("desc");
				for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
				{
					const std::string dataType = itarray->first;
					
					auto data = getDataFromComposer(dataType, itarray->second);

					res.push_back(data);
				}
				data::PatternData pattern(res);

				return static_cast<Data>(pattern);
			}

			static Data loadSquareCrop(const boost::property_tree::ptree& cropTree)
			{
				std::vector<Data> res;
				std::vector<int> pts = as_vector<int>(cropTree, "crop");
				auto pcitureJson = cropTree.get_child("IMGF");
				Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
				ImageData picture(static_cast<const ImageData &>(data_from_composer));
				
				filter::data::SquareCrop squareCrop(picture, pts);

				return squareCrop;
			}

			
			static Data getDataFromComposer(const std::string datatype, const boost::property_tree::ptree& dataNode)
			{
				IODataType ioDataType = DataTypeMapper::getTypeFromString(datatype);
				switch (ioDataType)
				{
				case IODataType::IMGF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImageFromFile(dataNode.get<std::string>("path"));
				case IODataType::VIDF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromFile(dataNode.get<std::string>("path"));
				case IODataType::SEQIMGD:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImagesFromDirectory(dataNode.get<std::string>("path"));
				case IODataType::STRMVID:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromStream(dataNode.get<std::string>("path"));
				case IODataType::LISTIO:
					filter::data::Composer::checkJsonFieldExist(dataNode, "array");
					return loadListIoData(dataNode);
				case IODataType::PATTERN:
					
					filter::data::Composer::checkJsonFieldExist(dataNode, "desc");
					return loadPatternData(dataNode);
				case IODataType::SQR_CROP:
					filter::data::Composer::checkJsonFieldExist(dataNode, "crop");
					filter::data::Composer::checkJsonFieldExist(dataNode, "IMGF");
					return loadSquareCrop(dataNode);
				case IODataType::NONE:
				default:
					throw HipeException("Cannot found the data type requested");
				}
			}

			static Data getDataFromComposer(const boost::property_tree::ptree& dataNode)
			{
				filter::data::Composer::checkJsonFieldExist(dataNode, "type");
				auto datatype = dataNode.get<std::string>("type");

				return getDataFromComposer(datatype, dataNode);
			}

		};
	}
}
