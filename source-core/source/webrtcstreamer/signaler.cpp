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


#include <iostream>
#include <string>

#include "scy/webrtc/audiopacketmodule.h"
#include "scy/webrtc/multiplexmediacapturer.h"
#include "scy/webrtc/videopacketsource.h"
#include "signaler.h"

#include "webrtc/api/mediastreamtrackproxy.h"


using std::endl;


namespace scy {


Signaler::Signaler(const smpl::Client::Options& options)
    : _client(options)
{
    // Setup the signalling client
    _client.StateChange += slot(this, &Signaler::onClientStateChange);
    _client.roster().ItemAdded += slot(this, &Signaler::onPeerConnected);
    _client.roster().ItemRemoved += slot(this, &Signaler::onPeerDiconnected);
    _client += packetSlot(this, &Signaler::onPeerMessage);
    _client.connect();

    // Setup a PeerConnectionFactory with our custom ADM
    _networkThread = rtc::Thread::CreateWithSocketServer();
    _workerThread = rtc::Thread::Create();
    if (!_networkThread->Start() || !_workerThread->Start())
        throw std::runtime_error("Failed to start threads");

    _factory = webrtc::CreatePeerConnectionFactory(
        _networkThread.get(), _workerThread.get(), rtc::Thread::Current(),
        //_capturer.getAudioModule(), nullptr, nullptr);
		nullptr, nullptr, nullptr);
}


Signaler::~Signaler()
{
}


void Signaler::startStreaming(const std::string& file, bool looping)
{
    // Open the video capture
    _capturer.openFile(file, looping);
    _capturer.start();
}


void Signaler::sendSDP(PeerConnection* conn, const std::string& type,
                       const std::string& sdp)
{
    assert(type == "offer" || type == "answer");
    smpl::Message m;
    json::value desc;
    desc[kSessionDescriptionTypeName] = type;
    desc[kSessionDescriptionSdpName] = sdp;
    m[type] = desc;

    postMessage(m);
}


void Signaler::sendCandidate(PeerConnection* conn, const std::string& mid,
                             int mlineindex, const std::string& sdp)
{
    smpl::Message m;
    json::value desc;
    desc[kCandidateSdpMidName] = mid;
    desc[kCandidateSdpMlineIndexName] = mlineindex;
    desc[kCandidateSdpName] = sdp;
    m["candidate"] = desc;

    postMessage(m);
}


void Signaler::onPeerConnected(smpl::Peer& peer)
{
    if (peer.id() == _client.ourID())
        return;
    DebugA("Peer connected: ", peer.id())

    if (PeerConnectionManager::exists(peer.id())) {
        DebugA("Peer already has session: ", peer.id())
        return;
    }

    // Create the Peer Connection
    auto conn = new PeerConnection(this, peer.id(), "", PeerConnection::Offer);
    conn->constraints().SetMandatoryReceiveAudio(false);
    conn->constraints().SetMandatoryReceiveVideo(false);
    conn->constraints().SetAllowDtlsSctpDataChannels();

    // Create the media stream and attach decoder  
    // output to the peer connection
    _capturer.addMediaTracks(_factory, conn->createMediaStream());

    // Send the Offer SDP
    conn->createConnection();
    conn->createOffer();

    PeerConnectionManager::add(peer.id(), conn);
}


void Signaler::onPeerMessage(smpl::Message& m)
{
    DebugA("Peer message: ", m.from().toString())

    if (m.find("offer") != m.end()) {
        assert(0 && "offer not supported");
    } else if (m.find("answer") != m.end()) {
        recvSDP(m.from().id, m["answer"]);
    } else if (m.find("candidate") != m.end()) {
        recvCandidate(m.from().id, m["candidate"]);
    }
    // else assert(0 && "unknown event");
}


void Signaler::onPeerDiconnected(const smpl::Peer& peer)
{
    DebugL << "Peer disconnected" << endl;

    auto conn = PeerConnectionManager::remove(peer.id());
    if (conn) {
        DebugA("Deleting peer connection: ", peer.id())
        // async delete not essential, but to be safe
        // delete conn;
        deleteLater<PeerConnection>(conn); 
    }
}


void Signaler::onClientStateChange(void*, sockio::ClientState& state, const sockio::ClientState& oldState)
{
    DebugA("Client state changed from ", oldState, " to ", state)

    switch (state.id()) {
        case sockio::ClientState::Connecting:
            break;
        case sockio::ClientState::Connected:
            break;
        case sockio::ClientState::Online:
            break;
        case sockio::ClientState::Error:
            throw std::runtime_error("Cannot connect to Symple server. "
                                     "Did you start the demo app and the "
                                     "Symple server is running on port 4500?");
    }
}


void Signaler::onAddRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream)
{
    assert(0 && "not required");
}


void Signaler::onRemoveRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream)
{
    assert(0 && "not required");
}


void Signaler::postMessage(const smpl::Message& m)
{
    _ipc.push(new ipc::Action(
        std::bind(&Signaler::syncMessage, this, std::placeholders::_1),
        m.clone()));
}


void Signaler::syncMessage(const ipc::Action& action)
{
    auto m = reinterpret_cast<smpl::Message*>(action.arg);
    _client.send(*m);
    delete m;
}


} // namespace scy
