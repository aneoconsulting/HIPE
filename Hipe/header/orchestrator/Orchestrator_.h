#pragma once
#include <core/Singleton.h>
#include <map>
#include <filter/Model.h>
#include <core/HipeException.h>
#include <json/JsonFilterNode/JsonFilterTree.h>

namespace orchestrator
{
	class OrchestratorBase
	{

	};

	template<class Conductor>
	class Orchestrator : public OrchestratorBase
						 
	{
		Conductor * _conductor;

	public:
		Conductor getConductor() const
		{
			return _conductor;
		}

		void setConductor(const Conductor * conductor)
		{
			_conductor = conductor;
		}

	public:
		Orchestrator(Conductor * conductor) : _conductor(conductor)
		{
			
		}

	};


	class OrchestratorFactory : public Singleton < OrchestratorFactory >
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
				throw HipeException(key_name + " Model already exist");
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

		template<>
		void addOrchestrator(KeyNameModel key_name, OrchestratorBase * orchestrator)
		{
			throw HipeException("Not yet implemented");
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

		static void start_orchestrator();

	};
}
