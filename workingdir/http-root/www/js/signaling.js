let serverToSendUrl = 'laptop-338:9001';
let serverToReceiverUrl = 'laptop-338:9002';
let socketToSend;

window.RTCPeerConnection = window.RTCPeerConnection ||
                           window.webkitRTCPeerConnection ||
                           window.mozRTCPeerConnection;
window.RTCSessionDescription = window.RTCSessionDescription ||
                               window.mozRTCSessionDescription;
window.RTCIceCandidate = window.RTCIceCandidate || 
                         window.mozRTCIceCandidate;

let pcToSend;
let pcToReceive;

window.onload = (e) => {
  socketToSend = new WebSocket('wss://' + serverToSendUrl);
  socketToSend.onopen = function () {
    console.log('socketToSend open');
    pcToSend = new RTCPeerConnection({ "iceServers": [
    {urls:'stun:stun.l.google.com:19302'} 
      ]}
    );

    // send any ice candidates to the other peer
    pcToSend.onicecandidate = function (evt) {
      if (evt.candidate) {
        socketToSend.send(JSON.stringify({ "candidate": evt.candidate }));
      }

    };

    // let the "negotiationneeded" event trigger offer generation
    pcToSend.onnegotiationneeded = function () {
      console.log('on negotiation needed for Sender!');
      createOffer(socketToSend, pcToSend);
    }

 
    // get a local stream, show it in a self-view and add it to be sent
    navigator.mediaDevices.getUserMedia({ "audio": false, "video": { width: 640, height: 480 } }, 
		// errorCallback
	   function(err) {
		if(err === PERMISSION_DENIED) {
		  console.log("Permission request refused");
		}
	   }
	)
    .then(function (stream) {
      selfView.srcObject = stream;
      if (typeof pcToSend.addTrack === "function") {
        pcToSend.addTrack(stream.getVideoTracks()[0], stream);
      } else {
        pcToSend.addStream(stream);
      }

      // createOffer(socketToSend);
      changeStatus("&#x25B6;");
      changeButton("&#x25B6;");
    })
    .catch(logError);
  };
  socketToSend.onmessage = function (evt) {
    console.log('on message from server.');
    let message = JSON.parse(evt.data);
    console.log(message);

    if (message.type === 'answer') {
      pcToSend.setRemoteDescription(new RTCSessionDescription(message),() => {
        socketToSend.send(JSON.stringify({type: 'join'}));
      }, logError);
    } else if (message.type === 'offer') {
      pcToSend.setRemoteDescription(new RTCSessionDescription(message), ()=>{
        createAnswer(socketToSend, pcToSend);
      }, logError);
    } else {
      // ICE candidate.
      pcToSend.addIceCandidate(new RTCIceCandidate(message.candidate), ()=>{}, logError);
    }
  };
  socketToSend.onclose = function () {
      console.log('closed socketToSend by server.');
  };
  socketToSend.onerror = function (e) {
    document.getElementById('status').innerText = 'websocket error.';
    console.log(e);
  };
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  socketToReceive = new WebSocket('wss://' + serverToReceiverUrl);
  socketToReceive.onopen = function () {
    console.log('socketToReceive open');
    pcToReceive = new RTCPeerConnection({ "iceServers": [
    {urls:'stun:stun.l.google.com:19302'} 
      ]}
    );

    // send any ice candidates to the other peer
    pcToReceive.onicecandidate = function (evt) {
      if (evt.candidate) {
		console.log('on ice candidate for receiver!');
        socketToReceive.send(JSON.stringify({ "candidate": evt.candidate }));
      }

    };

    // let the "negotiationneeded" event trigger offer generation
    pcToReceive.onnegotiationneeded = function () {
      console.log('on negotiation needed for receiver!');
      createOffer(socketToReceive);
    }
	
    // once remote stream arrives, show it in the remote video element
    pcToReceive.onaddstream = function (evt) {
      remoteView.srcObject = evt.stream;
    };
	
    pcToReceive.ontrack = function (evt) {
				console.log("ontrack.");
        if (evt.track.kind === "video")
          remoteView.srcObject = evt.streams[0];
    };

    // get a local stream, show it in a self-view and add it to be sent
    // navigator.mediaDevices.getUserMedia({ "audio": false, "video": { width: 640, height: 480 } })
    // .then(function (stream) {
      // selfView.srcObject = stream;
      // if (typeof pcToReceive.addTrack === "function") {
        // pcToReceive.addTrack(stream.getVideoTracks()[0], stream);
      // } else {
        // pcToReceive.addStream(stream);
      // }

      // // createOffer(socketToReceive);
      // changeStatus("&#x25B6;");
      // changeButton("&#x25B6;");
    // })
    // .catch(logError);
  };
  socketToReceive.onmessage = function (evt) {
    console.log('on message from Receiver server .');
    let message = JSON.parse(evt.data);
    console.log(message);

    if (message.type === 'answer') {
      pcToReceive.setRemoteDescription(new RTCSessionDescription(message),() => {
        socketToReceive.send(JSON.stringify({type: 'join'}));
      }, logError);
    } else if (message.type === 'offer') {
      pcToReceive.setRemoteDescription(new RTCSessionDescription(message), ()=>{
        createAnswer(socketToReceive, pcToReceive);
      }, logError);
    } else {
      // ICE candidate.
      pcToReceive.addIceCandidate(new RTCIceCandidate(message.candidate), ()=>{}, logError);
    }
  };
  socketToReceive.onclose = function () {
      console.log('closed socketToReceive by server.');
  };
  socketToReceive.onerror = function (e) {
    document.getElementById('status').innerText = 'websocket error.';
    console.log(e);
  };
};


function createOffer(socket, pc) {
  pc.createOffer()
  .then((desc) => {
    console.log(desc);
    pc.setLocalDescription(desc);
    socket.send(JSON.stringify(desc));
  })
  .catch(logError);
}

function createAnswer(socket, pc, desc) {
  pc.createAnswer()
  .then((desc) => {
    console.log(desc);
    pc.setLocalDescription(desc);
    socket.send(JSON.stringify(desc));
  })
  .catch(logError);
}

function logError(error) {
  console.log(error.name + ": " + error.message);
}

let mode = {
  onAir: false,
  isKaicho: false,
  useEye: false,
  useMsutache: false,
};
function switchMode(target) {
  mode[target] = !mode[target]; 
  socketToSend.send(JSON.stringify({"mode": mode}));
  socketToReceive.send(JSON.stringify({"mode": mode}));
}
function changeStatus(text) {
  document.getElementById('status').innerHTML = text;
}
function switchStart() {
  let text, target = 'onAir';
  switchMode(target);
  changeStatus('');
}
