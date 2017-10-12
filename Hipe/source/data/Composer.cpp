#include <data/Composer.h>

namespace filter
{
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
			data::PatternData pattern(res);

			return static_cast<Data>(pattern);
		}

		/**
		 * [TODO]
		 * \brief Extract the data from a json tree node (if existing) and load it to its corresponding type
		 * \param dataNode The node to query
		 * \return the loaded data (if existing) in its corresponding type (casted to the type Data)
		 */
		Data Composer::getDataFromComposer(const boost::property_tree::ptree& dataNode)
		{
			filter::data::Composer::checkJsonFieldExist(dataNode, "type");
			auto datatype = dataNode.get<std::string>("type");

			return getDataFromComposer(datatype, dataNode);
		}


	}
}
