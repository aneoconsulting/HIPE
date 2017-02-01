#include <orchestrator/Orchestrator.h>
#include <image/DefaultScheduler.h>






namespace orchestrator
{
	void OrchestratorFactory::start_orchestrator()
	{
		auto orchestrator_factory = OrchestratorFactory::getInstance();
		
		image::DefaultScheduler * default_scheduler = new image::DefaultScheduler();

		orchestrator_factory->addOrchestrator("DefaultScheduler", default_scheduler);
	}

}


template <class T> T* Singleton<T>::_instance = nullptr; 
template <class T> std::mutex Singleton<T>::_mutex; 
