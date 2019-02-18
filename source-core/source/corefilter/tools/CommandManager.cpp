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

#include <corefilter/tools/CommandManager.h>
#include "json/JsonTree.h"
#include "tools/RegisterTable.h"
#include "orchestrator/Orchestrator.h"
#include "core/version.h"

namespace corefilter
{

std::function<bool(std::string, json::JsonTree*)> get_filters()
{
	return [](std::string optionName, json::JsonTree* lptree)
	{
		const std::string filters = "Filters";
		int i = 0;
		if (filters.find(optionName) == 0)
		{
			RegisterTable& reg = RegisterTable::getInstance();
			for (auto& name : reg.getTypeNames())
			{
				json::JsonTree parameters;
				json::JsonTree child;
				json::JsonTree info;
				for (auto& varName : reg.getVarNames(name))
				{
					std::string value = reg.getDefaultValue(name, varName);
					child.put(varName, value);
				}
				info.put("namespace", reg.getNamespace(name));
				parameters.push_back("fields", child);
				parameters.push_back("info", info);

				lptree->add_child(name, parameters);
				++i;
			}
			return true;
		}
		return false;
	};
}

std::vector<std::string> splitfilterNamespaces(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

std::map<std::string, std::vector<json::JsonTree>> get_map_filters()
{
	std::map<std::string, std::vector<json::JsonTree>> tree;

	int i = 0;
	RegisterTable& reg = RegisterTable::getInstance();
	for (auto& name : reg.getTypeNames())
	{
		auto name_spacce_filter = reg.getNamespace(name);
		json::JsonTree filterNode;

		for (auto& varName : reg.getVarNames(name))
		{
			filterNode.put(varName, std::string(""));
		}
		json::JsonTree child;
		child.push_back(name, filterNode);

		tree[name_spacce_filter].push_back(child);
	}
	return tree;
}

//! \brief new get filters: each path is splitted on directories
//! \todo test when all namepaces will be set => change function name to get_filters() (remove new keyword)
std::function<bool(std::string, json::JsonTree*)> get_groupFilter()
{
	return [](std::string optionName, json::JsonTree* lptree)
	{
		const std::string filters = "GroupFilters";
		int i = 0;
		if (filters.find(optionName) == 0)
		{
			auto map_tree = get_map_filters();
			for (auto& mt : map_tree)
			{
				auto keys = splitfilterNamespaces(mt.first, '/');
				auto length = keys.size();
				auto lastElement = keys[length - 1];
				json::JsonTree lastElementJson;
				auto values = mt.second;
				for (auto& v : values)
				{
					lastElementJson.push_back("", v);
				}
				json::JsonTree* elements = new json::JsonTree[length];

				elements[length - 1] = lastElementJson;
				int i = 0;
				for (i = length - 2; i >= 0; i--)
				{
					json::JsonTree element;
					element.add_child(keys[i + 1], elements[i + 1]);
					if (lptree->count(keys[i]) == 1)
					{ 
						lptree->add_child_to_child(keys[i], keys[i + 1], elements[i + 1]);
					}
					else
					{
						lptree->add_child(keys[i], element);
					}
					elements[i] = element;
				}
				if (lptree->count(keys[0]) == 0)
					lptree->add_child(keys[0], elements[0]);
			}

			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree*)> get_version()
{
	return [](std::string optionName, json::JsonTree* lptree)
	{
		const std::string version = "Version";
		if (version.find(optionName) == 0)
		{
			auto v = getVersion();
			lptree->Add("Version", v);

			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree*)> get_versionHashed()
{
	return [](std::string optionName, json::JsonTree* lptree)
	{
		const std::string version = "Hash";
		if (version.find(optionName) == 0)
		{
			auto v = getVersionHashed();
			lptree->Add("Hash", v);
			return true;
		}
		return false;
	};
}

std::function<bool(std::string, json::JsonTree*)> get_commands_help()
{
	return [](std::string OptionName, json::JsonTree* lptree)
	{
		const std::string help = "Help";
		if (help.find(OptionName) == 0)
		{
			lptree->Add("Version", " returns the running app version number");
			lptree->Add("Hash", " returns the running app hashed version number ");
			lptree->Add("Exit", " stop the request");
			lptree->Add("Kill", " kills the current request");
			lptree->Add("Filters", " get all existing filters in the current version");
			lptree->Add("GroupFilters", "get all existing filter in groups");
			return true;
		}
		return false;
	};
}


}
