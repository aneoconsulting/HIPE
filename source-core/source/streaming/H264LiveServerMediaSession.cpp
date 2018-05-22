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

#include "H264LiveServerMediaSession.h"


H264LiveServerMediaSession* H264LiveServerMediaSession::createNew(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<data::Data> & concurrent_queue)
{
	return new H264LiveServerMediaSession(env, reuseFirstSource, concurrent_queue);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<data::Data> & concurrent_queue) : OnDemandServerMediaSubsession(env, reuseFirstSource), 
fAuxSDPLine(NULL), fDoneFlag(0), fDummySink(NULL), _concurrent_queue(concurrent_queue)
{
	
}


H264LiveServerMediaSession::~H264LiveServerMediaSession(void)
{
	delete[] fAuxSDPLine;
}


static void afterPlayingDummy(void* clientData)
{
	H264LiveServerMediaSession *session = (H264LiveServerMediaSession*)clientData;
	session->afterPlayingDummy1();
}

void H264LiveServerMediaSession::afterPlayingDummy1()
{
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData)
{
	H264LiveServerMediaSession* session = (H264LiveServerMediaSession*)clientData;
	session->checkForAuxSDPLine1();
}

void H264LiveServerMediaSession::checkForAuxSDPLine1()
{
	char const* dasl;
	if (fAuxSDPLine != NULL)
	{
		setDoneFlag();
	}
	else if (fDummySink != NULL && (dasl = fDummySink->auxSDPLine()) != NULL)
	{
		fAuxSDPLine = strDup(dasl);
		fDummySink = NULL;
		setDoneFlag();
	}
	else
	{
		int uSecsDelay = 100000;
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsDelay, (TaskFunc*)checkForAuxSDPLine, this);
	}
}

char const* H264LiveServerMediaSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
	if (fAuxSDPLine != NULL) return fAuxSDPLine;
	if (fDummySink == NULL)
	{
		fDummySink = rtpSink;
		fDummySink->startPlaying(*inputSource, afterPlayingDummy, this);
		checkForAuxSDPLine(this);
	}

	envir().taskScheduler().doEventLoop(&fDoneFlag);
	return fAuxSDPLine;
}

FramedSource* H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionID, unsigned& estBitRate)
{
	// Based on encoder configuration i kept it 90000
	estBitRate = 90000;
	LiveSourceWithx264 *source = LiveSourceWithx264::createNew(envir(), _concurrent_queue);
	// are you trying to keep the reference of the source somewhere? you shouldn't.  
	// Live555 will create and delete this class object many times. if you store it somewhere  
	// you will get memory access violation. instead you should configure you source to always read from your data source
	return H264VideoStreamDiscreteFramer::createNew(envir(), source);
}

RTPSink* H264LiveServerMediaSession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
