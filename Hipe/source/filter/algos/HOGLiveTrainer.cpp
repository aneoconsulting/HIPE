#include <filter/algos/LiveDetection.h>

HipeStatus filter::algos::LiveDetection::process()
{
	data::Data data = _connexData.pop();

	_connexData.push(data::ImageData(data));

	return OK;
}

void filter::algos::LiveDetection::dispose()
{
	_isThreadRunning = false;

	if (_pFilterThread != nullptr)
	{
		_pFilterThread->join();
		delete _pFilterThread;
		_pFilterThread = nullptr;
	}
}

void filter::algos::LiveDetection::startFilterThread()
{
	LiveDetection* pThis = this;
	_pFilterThread = new boost::thread([pThis]
	{
		while (pThis->_isThreadRunning)
		{
			data::Data data;
			if (!pThis->_inputDataStack.trypop_until(data, 30))		// Try to pop during 30s
				continue;

			data::Data output = pThis->processLiveTraining(data);

			if (pThis->_outputDataStack.size() != 0)
				pThis->_outputDataStack.clear();

			pThis->_outputDataStack.push(output);
		}
	});
}

filter::data::Data filter::algos::LiveDetection::processLiveTraining(const data::Data& data)
{
	return data::Data();
}
