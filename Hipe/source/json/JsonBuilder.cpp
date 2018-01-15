#include <JsonBuilder.h>
#include <data/OutputData.h>
#include <data/ImageArrayData.h>
#include <data/ImageEncodedData.h>
#include <boost/property_tree/ptree.hpp>

namespace json
{

	json::JsonFilterTree * JsonBuilder::buildAlgorithm(std::stringstream& dataResponse, json::JsonTree & treeRequest)
	{
		std::string OK = "Request OK";

		if (treeRequest.count("name") == 0)
			throw HipeException("The algorithm name is not found in the Json. Please inform the field \"name\" : \"name of algorithm\"");

		std::string algoName = treeRequest.get("name");


		auto filters = treeRequest.get_child("filters");

		auto tree = new JsonFilterTree();;
		tree->setName(algoName);
		

		for (auto filter = filters.begin(); filter != filters.end(); ++filter)
		{
			auto element = filter->second.begin(); //first and unique element of filter
			auto child = new JsonTree(element->second);
			auto type = element->first;
			auto name = child->get("name");
			auto res = static_cast<filter::Model *>(newFilter(type));
			res->setName(name);
			auto json_filter_node = JsonFilterNode(res, *child);
			json_filter_node.applyClassParameter();

			tree->add(json_filter_node);


			//TESTER
			dataResponse << type;
			dataResponse << " ";
			dataResponse << child->get("name");

			dataResponse << "; ";

		}
		try
		{
			tree->freeze();
		}
		catch (HipeException & e)
		{
			throw;
		}
		
		return tree;
	}


	

	template <>
	json::JsonTree JsonBuilder::buildJson<data::OutputData>(const data::OutputData & data)
	{
		json::JsonTree resultTree;
		json::JsonTree outputTree;

		// Case where there's no output data to process
		if (data.empty())
		{
			outputTree.Add("info", "NO Data as response");
			resultTree.AddChild("DataResult", outputTree);
			return resultTree;
		}
		if (data.This_const().getInput().getType() != data::IMGF && data.This_const().getInput().getType() != data::IMGENC)
		{
			outputTree.Add("ERROR", "Previous filter give wrong data type");
			resultTree.AddChild("DataResult", outputTree);
			return resultTree;
		}
		

		int data_index = 0;

		// For each image output its data in base64
		const data::ImageArrayData& imgdata = static_cast<const data::ImageArrayData &>(data.This_const().getInput());

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

			std::string typeValue = data::DataTypeMapper::getStringFromType(data.This_const().getType());

			outputTree.Add(typeKey.str(), typeValue);

			if (imgdata.getType() == data::IMGF)
			{
				outputTree.Add(formatKey.str(), "RAW");
				outputTree.AddInt(widthKey.str(), mat.cols);
				outputTree.AddInt(heightKey.str(), mat.rows);
				outputTree.AddInt(channelsKey.str(), mat.channels());
			}
			else if (imgdata.getType() == data::IMGENC)
			{
				const data::ImageEncodedData& imgEncData = static_cast<const data::ImageEncodedData&>(data.This_const().getInput());

				outputTree.Add(formatKey.str(), imgEncData.getCompression());
				outputTree.AddInt(widthKey.str(), imgEncData.getWidth());
				outputTree.AddInt(heightKey.str(), imgEncData.getHeight());
				outputTree.AddInt(channelsKey.str(), imgEncData.getChannelsCount());
			}

			outputTree.Add(dataKey.str(), data.mat2str(mat));

			data_index++;
		}

		std::stringstream output;

		resultTree.AddChild("DataResult", outputTree);

		return resultTree;
	}
}
