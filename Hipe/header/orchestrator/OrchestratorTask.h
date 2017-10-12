#pragma once
#include <data/IOData.h>
#include <memory>


namespace filter {
	class Model;
}

class OrchestratorTask
{
public:
	OrchestratorTask(filter::Model *root, filter::data::IOData& inputData, std::shared_ptr<filter::data::IOData> & outputData)
	{
		//Do a copy of the full Model to prevent of deletion during the thread execution


		//Do the same copy of input data and for output dataBe carefull need to downcast the data 

	}

	void runTask()
	{
		
	}

};
