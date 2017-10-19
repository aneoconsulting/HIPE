#include <orchestrator/Orchestrator.h>
#include <image/DefaultScheduler.h>
#include <core/misc.h>


template<> orchestrator::OrchestratorFactory* Singleton<orchestrator::OrchestratorFactory>::_instance = nullptr;

namespace orchestrator
{
	void OrchestratorFactory::start_orchestrator()
	{
		auto orchestrator_factory = OrchestratorFactory::getInstance();
		
		image::DefaultScheduler * default_scheduler = new image::DefaultScheduler();

		orchestrator_factory->addOrchestrator("DefaultScheduler", default_scheduler);

		
	}

	



}
