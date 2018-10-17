let socketToSend = null;
let socketToReceive = null;


window.RTCPeerConnection = window.RTCPeerConnection ||
                           window.webkitRTCPeerConnection ||
                           window.mozRTCPeerConnection;
window.RTCSessionDescription = window.RTCSessionDescription ||
                               window.mozRTCSessionDescription;
window.RTCIceCandidate = window.RTCIceCandidate || 
                         window.mozRTCIceCandidate;
                         
let pcToSend = null;
let pcToReceive = null;
let requestWebRTCSource = false;
let requestWebRTCOutput = false;
let target_port = 9001;
let source_port = 9002;

let mode = {
  onAir: false

};

var notifySuccess = function (msg) {
    console.log(msg)
};

var notifyError = function (msg) {
    if (msg == undefined || msg == null) {
        msg = "Une erreur est survenu. Veuillez vérifier vos informations."
    }

   console.log(msg)
};

function loading(elementIdName, canvasId)
{
    var circle = new Sonic({

        width: 50,
        height: 50,
        padding: 50,

        strokeColor: '#FFF',

        pointDistance: .01,
        stepsPerFrame: 3,
        trailLength: .7,

        step: 'fader',

        setup: function() {
            this._.lineWidth = 5;
        },

        path: [
            ['arc', 25, 25, 25, 0, 360]
        ]

    });

    circle.play();

    circle.canvas.id = canvasId;
    
    if (document.getElementById(canvasId) == null)
    {
        document.getElementById(elementIdName).append(circle.canvas);
        document.getElementById(elementIdName).append(circle.canvas);
    }
}
 


function GetConnectWebRTCServer(port)
{
    document.getElementById("selfView").style.visibility = "visible";
    document.getElementById("selfView").style.width = 128;
    document.getElementById("selfView").style.height = 96;
    document.getElementById('LoadingCanvasSrc').style.visibility = 'visible';
	 
	socketToSend = new WebSocket('wss://' + window.location.hostname + ":" + port);
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
        document.getElementById('LoadingCanvasSrc').style.visibility = 'hidden';

      if (typeof pcToSend.addTrack === "function") {
        pcToSend.addTrack(stream.getVideoTracks()[0], stream);
      } else {
        pcToSend.addStream(stream);
      }

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
      if (pcToSend != null) pcToSend.close();
      pcToSend = null;
  };
  socketToSend.onerror = function (e) {
      
      pcToSend = null;

    console.log(e);
  };
   
}

function swapDiv(event,elem){
    elem.parentNode.insertBefore(elem,elem.parentNode.firstChild);
}

function GetConnectWebRTCSender(port)
{
     
     document.getElementById('remoteView').style.visibility = "visible";
     document.getElementById("remoteView").style.width = 640;
     document.getElementById("remoteView").style.height = 480;
     
    socketToReceive = new WebSocket('wss://' + window.location.hostname + ":" + port);
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
        {
          remoteView.srcObject = evt.streams[0];
          document.getElementById('LoadingCanvasRemote').style.visibility = 'hidden';
        }
	  
    };
    
    mode.onAir = true;
    
	socketToReceive.send(JSON.stringify({"mode": mode}));
  }
  
  
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
      if (pcToReceive != null) pcToReceive.close();
      pcToReceive = null;
  };
  socketToReceive.onerror = function (e) {
    if (pcToReceive != null) pcToReceive.close();
    pcToReceive = null;
    console.log(e);
  };
}


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


function sleep_ms(millisecs) {
    var initiation = new Date().getTime();
    while ((new Date().getTime() - initiation) < millisecs);
}


window.onload = function() {
	loading('videoStream', 'LoadingCanvasRemote');
    loading('videoStream', 'LoadingCanvasSrc');
};
