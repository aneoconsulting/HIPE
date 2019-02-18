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

#include <liveMedia.hh>
#include <OnDemandServerMediaSubsession.hh>
#include <LiveSourceWithx264.h>


class H264LiveServerMediaSession :public OnDemandServerMediaSubsession
{
public:
	static H264LiveServerMediaSession* createNew(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<data::Data> & concurrent_queue);
	void checkForAuxSDPLine1();
	void afterPlayingDummy1();
protected:
	H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<data::Data> & concurrent_queue);
	virtual ~H264LiveServerMediaSession(void);
	void setDoneFlag() { fDoneFlag = ~0; }
protected:
	virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
private:
	char* fAuxSDPLine;
	char fDoneFlag;
	RTPSink* fDummySink;
	core::queue::ConcurrentQueue<data::Data> & _concurrent_queue;
};
