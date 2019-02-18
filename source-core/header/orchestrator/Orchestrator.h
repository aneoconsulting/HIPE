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

#pragma once
#include <cctype>
#include <map>
#include <iostream>
#include <type_traits>
#include <core/Singleton.h>
#include <corefilter/Model.h>
#include <core/HipeException.h>
#include <corefilter/tools/JsonFilterNode/JsonFilterTree.h>
#include <coredata/Data.h>
#include <orchestrator/orchestrator_export.h>
#include "TaskInfo.h"

namespace orchestrator
{

	class Conductor
	{

	};

	class OrchestratorBase
	{
	public:
		virtual void process(filter::Model* root, data::Data & data, data::Data & outPutData) = 0;

		virtual ~OrchestratorBase()
		{
			
		}

		virtual void killall()
		{
			
		}

		virtual std::vector<TaskInfo> getRunningTasks()
		{
			throw HipeException("Need to implement getRunningTasks into to the new Scheduler");
		}

	};
	
	template<class Conduct>
	class Orchestrator : public OrchestratorBase
						 
	{
		BOOST_STATIC_ASSERT((std::is_base_of<orchestrator::Conductor, Conduct>::value));

		Conduct * _conductor;

	public:
		Conduct getConductor() const
		{
			return _conductor;
		}

		void setConductor(const Conduct * conductor)
		{
			_conductor = conductor;
		}

	public:
		Orchestrator(Conduct * conductor) : _conductor(conductor)
		{
			
		}

		void process(filter::Model* root, data::Data & data, data::Data & outPutData)
		{
			
			_conductor->process(root, data, outPutData);
		}

		virtual void killall()
		{
			_conductor->killall();
		}

		virtual std::vector<TaskInfo> getRunningTasks()
		{
			return _conductor->getRunningTasks();
		}
	};

	
	class ORCHESTRATOR_EXPORT OrchestratorFactory : public Singleton < OrchestratorFactory >
	{
		friend class Singleton<OrchestratorFactory>;

	public:
		typedef std::string KeyNameModel;
		typedef std::string KeyNameOrchestrator;

	private:
		std::map < KeyNameOrchestrator, OrchestratorBase*> _orchestrators;
		std::map < KeyNameModel, filter::Model*> _models;

		std::map <KeyNameOrchestrator, KeyNameOrchestrator> _modelStore;

	protected:
		OrchestratorFactory() {}


	public:
		void addModel(KeyNameModel key_name, filter::Model * model)
		{
			if (_models.find(key_name) == _models.end())
			{
				_models[key_name] = model;
			}
			else
			{
				//throw HipeException(key_name + " Model already exist");
				//TODO Check if Algo need to be overriden
				if (model != nullptr)
					_models[key_name] = model;
			}
		}

		void addModel(KeyNameModel key_name_model, filter::Model * model, 
					  KeyNameOrchestrator key_name_orchesta, OrchestratorBase * orchestrator)
		{
			addModel(key_name_model, model);
			addOrchestrator(key_name_orchesta, orchestrator);

			_modelStore[key_name_model] = key_name_orchesta;
		}

		void addModel(KeyNameModel key_name_model, filter::Model * model,
					  KeyNameOrchestrator key_name_orchesta)
		{


			addModel(key_name_model, model);

			if (_orchestrators.find(key_name_orchesta) == _orchestrators.end())
			{
				throw HipeException("cannot find orchestrator" + key_name_orchesta);
			}

			_modelStore[key_name_model] = key_name_orchesta;
		}

		void addOrchestrator(KeyNameModel key_name, OrchestratorBase * orchestrator)
		{
			throw HipeException("Not yet implemented");
		}


		template <class Conductor>
		void addOrchestrator(KeyNameModel key_name, Conductor * conductor)
		{
			if (_orchestrators.find(key_name) == _orchestrators.end())
			{
				_orchestrators[key_name] = new Orchestrator<Conductor>(conductor);
			} 
			else
			{
				throw HipeException(key_name + " Orchestrator already exist");
			}
		}

		
		filter::Model* getModel(const std::string& model_name)
		{
			if (_models.find(model_name) != _models.end())
			{
				return _models[model_name];
			}
			else
			{
				throw HipeException(model_name + " model cannot be found");
			}
			return nullptr;
		}

		OrchestratorBase* getDefaultOrchestrator()
		{
			return _orchestrators.begin()->second;
			
		}

		OrchestratorBase* getOrchestrator(const std::string& model_name)
		{
			if (_modelStore.find(model_name) != _modelStore.end())
			{
				return _orchestrators[_modelStore[model_name]];
			}
			else
			{
				throw HipeException(model_name + " orchestrator cannot be found in the store");
			}
		}

		void bindModel(const std::string& modelName, const std::string& orchestratorName)
		{
			_modelStore[modelName] = orchestratorName;
		}

		void process(const std::string& model_name, data::Data & data, data::Data & outputData)
		{
			filter::Model * root;

			::json::JsonFilterTree * treeFilter;
			treeFilter = dynamic_cast<::json::JsonFilterTree *>(_models[model_name]);
			if (treeFilter != nullptr)
			{
				root = treeFilter->getRootNode();
				std::string keyname = model_name;
				std::replace_if(keyname.begin(), keyname.end(),
					std::not1(std::ptr_fun<int, int>(&std::isalnum)),
					'_'
				);
				root->setName(keyname);

			}
			else
				throw HipeException("Other Tree model not yet implemented");

			auto orchestrator_base = getOrchestrator(model_name);
			
			orchestrator_base->process(root, data, outputData);
		}

		void killall()
		{
			for (auto&pair : _orchestrators)
			{
				OrchestratorBase* orchestrator_base = pair.second;
				if (orchestrator_base != nullptr)
				{
					orchestrator_base->killall();
				}

			}
		}

		static void start_orchestrator();

	};

	
	std::function<bool(std::string, json::JsonTree*)> kill_command();

	std::function<bool(std::string, json::JsonTree*)> exit_command();

}
