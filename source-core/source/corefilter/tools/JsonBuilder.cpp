//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <tools/JsonBuilder.h>
#include "tools/JsonFilterNode/JsonFilterTree.h"
#include <coredata/OutputData.h>


#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>

#pragma warning(pop)

namespace json
{
	json::JsonFilterTree* JsonBuilder::buildAlgorithm(std::stringstream& dataResponse, json::JsonTree& treeRequest)
	{
		std::string OK = "Request OK";

		if (treeRequest.count("name") == 0)
			throw HipeException("The algorithm name is not found in the Json. Please inform the field \"name\" : \"name of algorithm\"");

		std::string algoName = treeRequest.get("name");


		auto filters = treeRequest.get_child("filters");

		auto tree = new JsonFilterTree();;
		tree->setName(algoName);

		//Look for datasource from data
		auto datas = treeRequest.get_child("data");
		if (datas.count("datasource") != 0)
		{
			datas = datas.get_child("datasource");
			for (auto data = datas.begin(); data != datas.end(); ++data)
			{
				auto element = data->second.begin(); //first and unique element of filter
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
		}
		//Build all node independently
		std::vector<JsonFilterNode> allNodes;
		
		for (auto filter = filters.begin(); filter != filters.end(); ++filter)
		{
			auto element = filter->second.begin(); //first and unique element of filter
			auto child = new JsonTree(element->second);
			auto type = element->first;
			auto name = child->get("name");
			auto res = static_cast<filter::Model *>(newFilter(type));
			res->setName(name);
			JsonFilterNode json_filter_node = JsonFilterNode(res, *child);
			json_filter_node.applyClassParameter();
			allNodes.push_back(json_filter_node);

			//If there some "__end_timer___" then disable perftim Plugins to generate endTimer 
			// because the object is already created
			if (json_filter_node.getFilter()->getName().find("__end_timer___") != std::string::npos)
			{
				tree->disablePerfTimerBuilder();
			}
		}

		for (auto json_filter_node : allNodes)
		{
			tree->add(json_filter_node);


			//TESTER
			dataResponse << json_filter_node.getFilter()->getConstructorName();
			dataResponse << " ";
			dataResponse << json_filter_node.getFilter()->getName();

			dataResponse << "; ";
		}
		try
		{
			tree->freeze();
		}
		catch (HipeException& e)
		{
			throw;
		}

		return tree;
	}


	template <>
	json::JsonTree JsonBuilder::buildJson<data::OutputData>(const data::OutputData& data)
	{
		json::JsonTree resultTree;
		json::JsonTree outputTree;

		throw HipeException("JsonBuilder.cpp : Need to rethink the output of a image. Since there is a core data. The child of OutputData should use JsonTree directly in the filter modules");
		/*
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
		*/
		return resultTree;
	}
}
