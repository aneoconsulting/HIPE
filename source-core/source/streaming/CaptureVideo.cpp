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

#include <CaptureVideo.h>
#include <RTSPCapture.h>
#include <core/Singleton.h>
#include <streaming/streaming_export.h>
#include <stdexcept>

template<> CaptureVideoFactory * Singleton<CaptureVideoFactory>::_instance = nullptr;


 CaptureVideo* CaptureVideoFactory::getCaptureVideo(std::string& captureVideoName, const std::string &path)
{
	CaptureVideoFactory *captureVideoFactory = CaptureVideoFactory::getInstance();

	
	
	if (captureVideoName == "RTSPCapture")
	{
		captureVideoFactory->locking.lock();
		if (captureVideoFactory->_captureVideoTable.find(captureVideoName) == captureVideoFactory->_captureVideoTable.end())
			captureVideoFactory->_captureVideoTable[captureVideoName] = new RTSPCapture(path);
		
		captureVideoFactory->locking.unlock();
	}
	else
	{
		captureVideoFactory->locking.unlock();
		throw HipeException("No captureVideo IOData is named : " + captureVideoName);
	}

	CaptureVideo* capture_video = captureVideoFactory->_captureVideoTable[captureVideoName];

	return capture_video;
}
