#pragma once

#include <map>
#include <iostream>
#include <type_traits>
#include <core/Singleton.h>
#include <filter/Model.h>
#include <core/HipeException.h>
#include <json/JsonFilterNode/JsonFilterTree.h>
#include <data/IOData.h>
#include <data/OutputData.h>
#include <orchestrator/orchestrator_export.h>

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

		void killall()
		{
			_conductor->killall();
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

			}
			else
				throw HipeException("Other Tree model not yet implemented");

			auto orchestrator_base = getOrchestrator(model_name);
			
			orchestrator_base->process(root, data, outputData);
		}

		void killall()
		{
			auto orchestrator_base = getDefaultOrchestrator();
			orchestrator_base->killall();
		}

		static void start_orchestrator();

	};


}
