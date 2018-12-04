//@HIPE_LICENSE@
#include <orchestrator/Orchestrator.h>
#include <image/DefaultScheduler.h>
#include <image/SchedulerSimonV2.h>
#include <core/misc.h>


template <>
orchestrator::OrchestratorFactory* Singleton<orchestrator::OrchestratorFactory>::_instance = nullptr;

namespace orchestrator
{
	std::function<bool(std::string, json::JsonTree*)> kill_command()
	{
		return [](std::string optionName, json::JsonTree* lptree)
		{
			const std::string kill = "Kill";
			if (kill.find(optionName) == 0)
			{
				orchestrator::OrchestratorFactory::getInstance()->killall();
				lptree->Add("Status", "Task has been killed");
				return true;
			}
			return false;
		};
	}

	std::function<bool(std::string, json::JsonTree*)> exit_command()
	{
		return [](std::string optionName, json::JsonTree* lptree)
		{
			const std::string exit = "Exit";
			if (exit.find(optionName) == 0)
			{
				orchestrator::OrchestratorFactory::getInstance()->killall();
				lptree->Add("Status", "Task has been killed");
				lptree->Add("process", "Server is exiting");
				return true;
			}
			return false;
		};
	}


	void OrchestratorFactory::start_orchestrator()
	{
		auto orchestrator_factory = OrchestratorFactory::getInstance();

		image::DefaultScheduler* default_scheduler = new image::DefaultScheduler();

		orchestrator_factory->addOrchestrator("DefaultScheduler", default_scheduler);

		image::SchedulerSimonV2* Version2_Scheduler_Simon = new image::SchedulerSimonV2();

		orchestrator_factory->addOrchestrator("SchedulerSimonV2", Version2_Scheduler_Simon);
	}
}
