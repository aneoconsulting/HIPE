#include <filter/algos/HOGLiveTrainer.h>

HipeStatus filter::algos::HOGLiveTrainer::process()
{
	data::ImageData data = _connexData.pop();

	skipFrames();
	pushInputFrame(data);

	_connexData.push(popOutputData());

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
		pThis->_ht.set_training_images_per_second(pThis->training_images_frequency);
		pThis->_ht.set_number_training_threads(pThis->training_threads);
		pThis->_ht.set_fhog_epsilon(pThis->fhog_epsilon);
		pThis->_ht.set_configuration_interval(pThis->configuration_interval);
		pThis->_ht.configure_viewport_from_parameters(pThis->viewport_size_x, pThis->viewport_size_y, pThis->viewport_offset_x, pThis->viewport_offset_y);

		pThis->_ht.init();

		data::DlibDetectorData output;
		data::ImageData data;

		std::vector<dlib::object_detector<data::hog_trainer::image_scanner_type> > activeDetectors;

		while (pThis->_isThreadRunning)
		{
			if (pThis->_inputDataStack.trypop_until(data, 30))		// Try to pop during 30s
			{
				pThis->_ht.process_frame(data.getMat());
				activeDetectors.clear();
				pThis->_ht.get_active_detectors(activeDetectors);
			}

			output.detectors() = activeDetectors;

			if (pThis->_outputDataStack.size() != 0)
				pThis->_outputDataStack.clear();

			pThis->_outputDataStack.push(output);
		}
	});
}

void filter::algos::HOGLiveTrainer::pushInputFrame(const data::ImageData& frame)
{
	data::ImageData frameCopy(frame.getMat().clone());
	_inputDataStack.push(frameCopy);
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
