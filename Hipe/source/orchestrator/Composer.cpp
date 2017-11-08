#include <data/IOData.h>
#include <json/JsonTree.h>
#include <Composer.h>
#include "data/DirPatternData.h"

namespace orchestrator
{
	filter::data::Data orchestrator::Composer::loadListIoData(const json::JsonTree& dataNode)
	{
		using namespace filter::data;
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
		using namespace filter::data;
		bool isDirPAtterData=false;
		std::vector<Data> res;
		auto child = dataNode.allchildren("desc");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			const std::string dataType = itarray->first;
			auto data = orchestrator::Composer::getDataFromComposer(dataType, *itarray->second);
			res.push_back(data);
			filter::data::IODataType ioDataType = filter::data::DataTypeMapper::getTypeFromString(dataType);
			/*if(ioDataType == IODataType::SEQIMGD)
			{
				isDirPAtterData = true;
			}*/
		}
		/*if(isDirPAtterData)
		{
			DirPatternData dirPattern(res);
			return static_cast<Data>(dirPattern);
		}*/
		filter::data::PatternData pattern(res);

		return static_cast<Data>(pattern);
	}

	/**
	 * [TODO]
	 * \brief Extract the data from a json tree node (if existing) and load it to its corresponding type
	 * \param dataNode The node to query
	 * \return the loaded data (if existing) in its corresponding type (casted to the type Data)
	 */
	filter::data::Data Composer::getDataFromComposer(const json::JsonTree& dataNode)
	{
		using namespace filter::data;
		checkJsonFieldExist(dataNode, "type");
		auto datatype = dataNode.get("type");

		return getDataFromComposer(datatype, dataNode);
	}

}
