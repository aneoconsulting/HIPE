#pragma once

#include <liveMedia.hh>
#include <OnDemandServerMediaSubsession.hh>
#include <LiveSourceWithx264.h>


class H264LiveServerMediaSession :public OnDemandServerMediaSubsession
{
public:
	static H264LiveServerMediaSession* createNew(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<filter::data::IOData> & concurrent_queue);
	void checkForAuxSDPLine1();
	void afterPlayingDummy1();
protected:
	H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource, core::queue::ConcurrentQueue<filter::data::IOData> & concurrent_queue);
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
	core::queue::ConcurrentQueue<filter::data::IOData> & _concurrent_queue;
};
