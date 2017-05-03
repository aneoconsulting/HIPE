#include <orchestrator/Orchestrator.h>
#include <image/DefaultScheduler.h>
#include <core/misc.h>

template<> DLL_PUBLIC orchestrator::OrchestratorFactory* Singleton<orchestrator::OrchestratorFactory>::_instance = nullptr; 
//template<> DLL_PUBLIC std::mutex Singleton<orchestrator::OrchestratorFactory>::_mutex;


namespace orchestrator
{
	void OrchestratorFactory::start_orchestrator()
	{
		auto orchestrator_factory = OrchestratorFactory::getInstance();
		
		image::DefaultScheduler * default_scheduler = new image::DefaultScheduler();

		orchestrator_factory->addOrchestrator("DefaultScheduler", default_scheduler);
	}




}
