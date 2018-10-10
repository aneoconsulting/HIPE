//@HIPE_LICENSE@
#pragma once
#include <thread>
#include <core/python/pyThreadSupport.h>
#include <coredata/OutputData.h>
#include <orchestrator/TaskInfo.h>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/datasource/DataSource.h>

#pragma warning(push, 0)
#include <boost/thread/thread.hpp>
#include <Python.h>
#include "Orchestrator.h"
#pragma warning(pop)


namespace orchestrator
{
	namespace image
	{
		class PyAllowThreads
		{
		public:
			PyAllowThreads() :
				_state(PyEval_SaveThread())
			{
			}

			~PyAllowThreads()
			{
				PyEval_RestoreThread(_state);
			}

		private:
			PyThreadState* _state;
		};

		

		class DefaultScheduler : public orchestrator::Conductor
		{
		public:
			typedef std::vector<std::vector<filter::Model *>> MatrixLayerNode;
			std::vector<TaskInfo> runningTasks;
			std::map<std::thread::id, PyThreadState*> pyMainThreaState;
			std::map<std::thread::id, PyThreadState*> pyThreadState;
			
			std::map<std::thread::id, PyInterpreterState*> interp;


			DefaultScheduler();

			static int getMaxLevelNode(filter::Model* filter, int level = 0);

			static int setMatrixLayer(filter::Model* filter, MatrixLayerNode& matrixLayerNode);

			static void popUnusedData(filter::Model* filter);

			static void cleanDataChild(filter::Model* filter);

			static void disposeChild(filter::Model* filter);

			void CallFiltersOnload(filter::Model* filter, int level = 0);

			static void InitNewPythonThread(PyInterpreterState* interpreterPython, PyExternalUser* & pyExternalUser);

			static void onStartCall(filter::Model* model, void* context);

			static void setPythonUserThreadState(filter::Model* model, PyExternalUser* py_external_user);

			static void destroyPythonThread(PyExternalUser* & pyExternalUser);


			void processDataSource(filter::Model* root, data::Data& outputData, bool debug);

			void updateFilterParameters(filter::Model* root, std::shared_ptr<filter::Model> model);

			void process(filter::Model* root, data::Data& inputData, data::Data& outputData, bool debug = false);

			virtual void killall();
		};
	}
}
