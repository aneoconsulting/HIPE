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

///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <http://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#ifndef SCY_WebRTC_WebRTCStreamer_Signaler_H
#define SCY_WebRTC_WebRTCStreamer_Signaler_H


#include "scy/application.h"
#include "scy/ipc.h"
#include "scy/net/sslmanager.h"
#include "scy/net/sslsocket.h"
#include "scy/symple/client.h"
#include "scy/util.h"
#include "scy/webrtc/peerconnectionmanager.h"
#include "scy/webrtc/multiplexmediacapturer.h"

#include "config.h"
#include "OpenCVMultipleMediaCapturer.h"


namespace scy {


class Signaler : public PeerConnectionManager, public Application
{
public:
    Signaler(const smpl::Client::Options& options);
    ~Signaler();

    void startStreaming(const std::string& file, bool loop = true);

	virtual core::queue::ConcurrentQueue<cv::Mat> & getQueueMat()
	{
		return _capturer.getQueueMat();
	}

protected:

    /// PeerConnectionManager interface
    void sendSDP(PeerConnection* conn, const std::string& type, const std::string& sdp);
    void sendCandidate(PeerConnection* conn, const std::string& mid, int mlineindex, const std::string& sdp);
    void onAddRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream);
    void onRemoveRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream);

    void postMessage(const smpl::Message& m);
    void syncMessage(const ipc::Action& action);

    void onPeerConnected(smpl::Peer& peer);
    void onPeerMessage(smpl::Message& m);
    void onPeerDiconnected(const smpl::Peer& peer);

    void onClientStateChange(void* sender, sockio::ClientState& state,
                             const sockio::ClientState& oldState);
	
protected:
    ipc::SyncQueue<> _ipc;
#if USE_SSL
    smpl::SSLClient _client;
#else
    smpl::TCPClient _client;
#endif
    //MultiplexMediaCapturer _capturer;
	OpenCVMultipleMediaCapturer _capturer;
    std::unique_ptr<rtc::Thread> _networkThread;
    std::unique_ptr<rtc::Thread> _workerThread;
};


} // namespace scy


#endif
