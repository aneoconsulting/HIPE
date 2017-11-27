#include <filter/algos/HOGLiveTrainer.h>

HipeStatus filter::algos::HOGLiveTrainer::process()
{
	data::ImageData data = _connexData.pop();

	skipFrames();
	pushInputFrame(data);

	data::DlibDetectorData output = popOutputData();
	_connexData.push(output);

	return OK;
}

void filter::algos::HOGLiveTrainer::dispose()
{
	_isThreadRunning = false;

	if (_pFilterThread != nullptr)
	{
		_pFilterThread->join();
		delete _pFilterThread;
		_pFilterThread = nullptr;
	}
}

void filter::algos::HOGLiveTrainer::startFilterThread()
{
	HOGLiveTrainer* pThis = this;
	_pFilterThread = new boost::thread([pThis]
	{
		//data::DlibDetectorData output(pThis->_ht.get_detectors(), pThis->_ht.get_mutex());

		pThis->_ht.init();
		while (pThis->_isThreadRunning)
		{
			data::ImageData data;
			if (!pThis->_inputDataStack.trypop_until(data, 30))		// Try to pop during 30s
				continue;

			pThis->_ht.process_frame(data.getMat());
			data::DlibDetectorData output(pThis->_ht.get_detectors());

			if (pThis->_outputDataStack.size() != 0)
				pThis->_outputDataStack.clear();
			pThis->_outputDataStack.push(output);
		}
	});
}

void filter::algos::HOGLiveTrainer::pushInputFrame(const data::ImageData& frame)
{
	_inputDataStack.push(frame);
	++_countProcessedFrames;
}

void filter::algos::HOGLiveTrainer::skipFrames()
{
	if (skip_frames == 0)
		return;

	if (_countProcessedFrames % skip_frames == 0)
	{
		_countProcessedFrames = 0;

		if (_inputDataStack.size() != 0)
			_inputDataStack.clear();
	}
}

data::DlibDetectorData filter::algos::HOGLiveTrainer::popOutputData()
{
	const int waitTime = 30;

	_outputDataStack.trypop_until(_outputData, waitTime);

	return _outputData;
}
