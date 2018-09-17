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
 



/**
 * inits a websocket by a given url, returned promise resolves with initialized websocket, rejects after failure/timeout.
 *
 * @param url the websocket url to init
 * @param existingWebsocket if passed and this passed websocket is already open, this existingWebsocket is resolved, no additional websocket is opened
 * @param timeoutMs the timeout in milliseconds for opening the websocket
 * @param numberOfRetries the number of times initializing the socket should be retried, if not specified or 0, no retries are made
 *        and a failure/timeout causes rejection of the returned promise
 * @return {Promise}
 */
function initWebsocket(url, existingWebsocket, timeoutMs, numberOfRetries, onOpenFunc, onCloseFunc, onErrorFunc) {
    timeoutMs = timeoutMs ? timeoutMs : 1500;
    numberOfRetries = numberOfRetries ? numberOfRetries : 0;
    var hasReturned = false;
    var promise = new Promise((resolve, reject) => {
        setTimeout(function () {
            if(!hasReturned) {
                console.info('opening websocket timed out: ' + url);
                rejectInternal();
            }
        }, timeoutMs);
        if (!existingWebsocket || existingWebsocket.readyState != existingWebsocket.OPEN) {
            if (existingWebsocket) {
                existingWebsocket.close();
            }
            var websocket = new WebSocket(url);
            websocket.onopen = function () {
                if(hasReturned) {
                    websocket.close();
                } else {
                    console.info('websocket to opened! url: ' + url);
                    resolve(websocket);
                }
            };
            websocket.onclose = function () {
                console.info('websocket closed! url: ' + url);
                if (onCloseFunc) 
                    onCloseFunc();
                
                rejectInternal();
            };
            websocket.onerror = function () {
                console.info('websocket error! url: ' + url);
                rejectInternal();
            };
        } else {
            resolve(existingWebsocket);
        }

        function rejectInternal() {
            if(numberOfRetries <= 0) {
                reject();
            } else if(!hasReturned) {
                hasReturned = true;
                console.info('retrying connection to websocket! url: ' + url + ', remaining retries: ' + (numberOfRetries-1));
                initWebsocket(url, null, timeoutMs, numberOfRetries-1).then(resolve, reject);
            }
        }
    });
    promise.then(function () {hasReturned = true;}, function () {hasReturned = true;});
    return promise;
};


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
    {urls:'stun:stun.l.google.com:19302'},
    {urls:'stun:stun1.l.google.com:19302'},
    {urls:'stun:stun2.l.google.com:19302'},
    {urls:'stun:stun3.l.google.com:19302'},
    {urls:'stun:stun4.l.google.com:19302'}
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
    {urls:'stun:stun.l.google.com:19302'},
    {urls:'stun:stun1.l.google.com:19302'},
    {urls:'stun:stun2.l.google.com:19302'},
    {urls:'stun:stun3.l.google.com:19302'},
    {urls:'stun:stun4.l.google.com:19302'}
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



function switchMode(target) {
  mode[target] = !mode[target]; 
  socketToSend.send(JSON.stringify({"mode": mode}));
  socketToReceive.send(JSON.stringify({"mode": mode}));
}
// function changeStatus(text) {
  // document.getElementById('status').innerHTML = text;
// }
function switchStart() {
  let text, target = 'onAir';
  switchMode(target);
  //changeStatus('');
}

function sleep_ms(millisecs) {
    var initiation = new Date().getTime();
    while ((new Date().getTime() - initiation) < millisecs);
}

function killTaskAndRun ( jsonData )
{
     if (pcToReceive != null) pcToReceive.close();
     pcToReceive = null;
     if (pcToSend != null) pcToSend.close();
     pcToReceive = null;
     socketToSend = null;
     socketToReceive = null;
    
     var filterBodyRaw = {
        "name": 'kill',

        "command":
            {
                "type": 'Kill'
            }
    };
    
    document.getElementById('remoteView').style.visibility = 'visible';
    document.getElementById('LoadingCanvasRemote').style.visibility = 'visible';
    document.getElementById('LoadingCanvasSrc').style.visibility = 'hidden';
    document.getElementById('selfView').style.visibility = 'hidden';
    
	$.ajax({
        data: JSON.stringify(filterBodyRaw),
        url: '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La commande n\'a pas pu être effectuée.');
        },
        success: function (data) {
				notifySuccess('La commande kill a été effectée.');
				$.ajax({
				crossOrigin: true,
				url: '/json',
				dataType: 'JSON',
				type: 'POST',
				contentType: 'application/json',
				
				data: JSON.stringify(jsonData),
				processData: false,
				success: function( data, textStatus, jQxhr ){
                    
					if (requestWebRTCSource == true)
					{
                       
                        document.getElementById('selfView').style.visibility = 'visible';
                        document.getElementById('LoadingCanvasSrc').style.visibility = 'visible';

                        //sleep_ms(2000);
						GetConnectWebRTCServer(target_port);
					} else{
                        document.getElementById('LoadingCanvasSrc').style.visibility = 'hidden';
                        document.getElementById('selfView').style.visibility = 'hidden';
                    }
					
					if (requestWebRTCOutput == true)
					{
                        sleep_ms(2000);
						GetConnectWebRTCSender(source_port);
					}
                   
			
				},
				error: function( jqXhr, textStatus, errorThrown ){
					console.log( errorThrown );
					 notifyError( errorThrown );
				}
			});
        }
	});
	
}

function processJSON( jsonData ){
        console.log( jsonData );
		killTaskAndRun ( jsonData );
}
		
function sendJsonRequest(jsonFilename)
{
	console.log(jsonFilename);
    requestWebRTCSource = false;
	
	$.getJSON(jsonFilename, function(json) {
		
		if (json['data'] != null && json['data']['datasource'] != null)
		{
			var dataSource = json['data']['datasource'];
			for (var i = 0; i < dataSource.length; i++)
			{
					if (dataSource[i].WebRTCVideoDataSource != null)
					{
						requestWebRTCSource = true;
						target_port = dataSource[i]['WebRTCVideoDataSource'].port;
						break;
					}
			}
			
		}
		
		if (json['filters'] != null)
		{
			
			var filters = json['filters'];
			for (var i = 0; i < filters.length; i++)
			{
					if (filters[i].WebRTCSender != null)
					{
						requestWebRTCOutput = true;
						source_port = filters[i]['WebRTCSender'].port;
						break;
					}
			}
		}
		
		processJSON(json)
		
		
		
		
		console.log();
	});

	return "";
}

window.onload = function() {
	loading('videoStream', 'LoadingCanvasRemote');
    loading('videoStream', 'LoadingCanvasSrc');
};
