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

			virtual std::vector<TaskInfo> getRunningTasks();
		};
	}
}
