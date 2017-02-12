#pragma once
#include <string>
#include "IOData.h"
#include "IODataType.h"
#include <core/HipeException.h>
#include <boost/property_tree/ptree.hpp>
#include "FileImageData.h"


namespace filter
{
	namespace data
	{
	

		class Composer
		{
		public:
			static inline void checkJsonFieldExist(boost::property_tree::ptree & jsonNode, std::string key)
			{
				if (jsonNode.count(key) == 0)
				{
					throw HipeException("Cannot find field json request. Requested field is : " + key);
				}
			}

			static IOData loadImageFromFile(std::string strPath)
			{
				FileImageData res(strPath);

				return res;
			}

			static IOData getDataFromComposer(boost::property_tree::ptree & dataNode)
			{
				std::string datatype = dataNode.get<std::string>("type");
				

				IODataType ioDataType = DataTypeMapper::getTypeFromString(datatype);
				switch (ioDataType)
				{
				case IODataType::IMGF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "type");
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImageFromFile(dataNode.get<std::string>("path"));

					break;
				case !(IODataType::IMGF || IODataType::NONE) :
					throw HipeException(datatype + " data type not yet implemented");
					break;
				case IODataType::NONE:
				default:
					throw HipeException("Cannot found the data type requested");
				}
			}
		};
	}
}
