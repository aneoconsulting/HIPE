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

#include <dlib/simd.h>
#include <filter/algos/detection/HOGLiveTrainer.h>

HipeStatus filter::algos::HOGLiveTrainer::process()
{
	data::ImageData data = _connexData.pop();

	skipFrames();
	pushInputFrame(data);

	PUSH_DATA(popOutputData());

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
