var socketToSend = null;
var socketToReceive = null;


window.RTCPeerConnection = window.RTCPeerConnection ||
    window.webkitRTCPeerConnection ||
    window.mozRTCPeerConnection;
window.RTCSessionDescription = window.RTCSessionDescription ||
    window.mozRTCSessionDescription;
window.RTCIceCandidate = window.RTCIceCandidate ||
    window.mozRTCIceCandidate;

var mode = {
    onAir: false

};

var cameras = [];
var front = 0;
var mediaStream = null;

var notifySuccess = function(msg) {
    console.log(msg)
};

var notifyError = function(msg) {
    if (msg == undefined || msg == null) {
        msg = "Une erreur est survenu. Veuillez vérifier vos informations."
    }

    console.log(msg)
};

function loading(elementIdName, canvasId) {
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

    if (document.getElementById(canvasId) == null) {
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
        setTimeout(function() {
                if (!hasReturned) {
                    console.info('opening websocket timed out: ' + url);
                    rejectInternal();
                }
            },
            timeoutMs);
        if (!existingWebsocket || existingWebsocket.readyState != existingWebsocket.OPEN) {
            if (existingWebsocket) {
                existingWebsocket.close();
            }
            var websocket = new WebSocket(url);
            websocket.onopen = function() {
                if (hasReturned) {
                    websocket.close();
                } else {
                    console.info('websocket to opened! url: ' + url);
                    resolve(websocket);
                }
            };
            websocket.onclose = function() {
                console.info('websocket closed! url: ' + url);
                if (onCloseFunc)
                    onCloseFunc();

                rejectInternal();
            };
            websocket.onerror = function() {
                console.info('websocket error! url: ' + url);
                rejectInternal();
            };
        } else {
            resolve(existingWebsocket);
        }

        function rejectInternal() {
            if (numberOfRetries <= 0) {
                reject();
            } else if (!hasReturned) {
                hasReturned = true;
                console.info('retrying connection to websocket! url: ' +
                    url +
                    ', remaining retries: ' +
                    (numberOfRetries - 1));
                initWebsocket(url, null, timeoutMs, numberOfRetries - 1).then(resolve, reject);
            }
        }
    });
    promise.then(function() { hasReturned = true; }, function() { hasReturned = true; });
    return promise;
};


navigator.mediaDevices.enumerateDevices()
    .then(gotDevices);


function gotDevices(deviceInfos) {
    cameras = [];
    for (var i = 0; i !== deviceInfos.length; ++i) {
        var deviceInfo = deviceInfos[i];
        if (deviceInfo.kind === 'audioinput') {
            // option.text = deviceInfo.label ||
            // 'microphone ' + (audioSelect.length + 1);
            // audioSelect.appendChild(option);
        } else if (deviceInfo.kind === 'videoinput') {
            cameras.push(deviceInfo.deviceId)

        } else {
            console.log('Found one other kind of source/device: ', deviceInfo);
        }
    }
}

function GetConnectWebRTCServer(videoObject) {
    var selfView = document.getElementById(videoObject.videoId);

    videoObject.socketToSend = new WebSocket('wss://' + window.location.hostname + ":" + videoObject.port);
    videoObject.socketToSend.onopen = function() {
        console.log('socketToSend open');
        videoObject.pcToSend = new RTCPeerConnection({
                "iceServers": [
                    { urls: 'stun:stun.l.google.com:19302' },
                    { urls: 'stun:stun1.l.google.com:19302' },
                    { urls: 'stun:stun2.l.google.com:19302' },
                    { urls: 'stun:stun3.l.google.com:19302' },
                    { urls: 'stun:stun4.l.google.com:19302' }
                ]
            }
        );

        // send any ice candidates to the other peer
        videoObject.pcToSend.onicecandidate = function(evt) {
            if (evt.candidate) {
                videoObject.socketToSend.send(JSON.stringify({ "candidate": evt.candidate }));
            }

        };

        // let the "negotiationneeded" event trigger offer generation
        videoObject.pcToSend.onnegotiationneeded = function() {
            console.log('on negotiation needed for Sender!');
            createOffer(videoObject.socketToSend, videoObject.pcToSend);
        }

        var constraints = {
            audio: false,
            video:
            {
                deviceId: { exact: front },
                width: 640,
                height: 480
            }
        };

        // get a local stream, show it in a self-view and add it to be sent
        navigator.mediaDevices.getUserMedia(constraints,
                function(stream) {

                    /*
                     * Rest of your code.....
                     * */
                },
                // errorCallback
                function(err) {
                    if (err === PERMISSION_DENIED) {
                        console.log("Permission request refused");
                    }
                }
            )
            .then(function(stream) {
                selfView.srcObject = stream;
                mediaStream = stream;
                mediaStream.stop = function() {
                    this.getAudioTracks().forEach(function(track) {
                        track.stop();
                    });
                    this.getVideoTracks().forEach(function(track) { //in case... :)
                        track.stop();
                    });
                };
                // document.getElementById('LoadingCanvasSrc').style.visibility = 'hidden';
                selfView.style.visibility = "visible";
                if (typeof videoObject.pcToSend.addTrack === "function") {
                    videoObject.pcToSend.addTrack(stream.getVideoTracks()[0], stream);
                } else {
                    videoObject.pcToSend.addStream(stream);
                }

            })
            .catch(logError);
    };
    videoObject.socketToSend.onmessage = function(evt) {
        console.log('on message from server.');
        var message = JSON.parse(evt.data);
        console.log(message);

        if (message.type === 'answer') {
            videoObject.pcToSend.setRemoteDescription(new RTCSessionDescription(message),
                () => {
                    videoObject.socketToSend.send(JSON.stringify({ type: 'join' }));
                },
                logError);
        } else if (message.type === 'offer') {
            videoObject.pcToSend.setRemoteDescription(new RTCSessionDescription(message),
                () => {
                    createAnswer(videoObject.socketToSend, videoObject.pcToSend);
                },
                logError);
        } else {
            // ICE candidate.
            videoObject.pcToSend.addIceCandidate(new RTCIceCandidate(message.candidate), () => {}, logError);
        }
    };
    videoObject.socketToSend.onclose = function() {
        console.log('closed socketToSend by server.');
        if (mediaStream) mediaStream.stop();
        if (videoObject.pcToSend != null) videoObject.pcToSend.close();
        videoObject.pcToSend = null;
    };
    videoObject.socketToSend.onerror = function(e) {
        if (mediaStream) mediaStream.stop();
        videoObject.pcToSend = null;

        console.log(e);
    };

}

function swapDiv(event, elem) {
    elem.parentNode.insertBefore(elem, elem.parentNode.firstChild);
}

function GetConnectWebRTCSender(videoObject) {
    var remoteView = document.getElementById(videoObject.videoId);


    videoObject.socketToReceive = new WebSocket('wss://' + window.location.hostname + ":" + videoObject.port);
    videoObject.socketToReceive.onopen = function() {
        console.log('socketToReceive open');
        videoObject.pcToReceive = new RTCPeerConnection({
                "iceServers": [
                    { urls: 'stun:stun.l.google.com:19302' },
                    { urls: 'stun:stun1.l.google.com:19302' },
                    { urls: 'stun:stun2.l.google.com:19302' },
                    { urls: 'stun:stun3.l.google.com:19302' },
                    { urls: 'stun:stun4.l.google.com:19302' }
                ]
            }
        );

        // send any ice candidates to the other peer
        videoObject.pcToReceive.onicecandidate = function(evt) {
            if (evt.candidate) {
                console.log('on ice candidate for receiver!');
                videoObject.socketToReceive.send(JSON.stringify({ "candidate": evt.candidate }));
            }

        };

        // let the "negotiationneeded" event trigger offer generation
        videoObject.pcToReceive.onnegotiationneeded = function() {
            console.log('on negotiation needed for receiver!');
            createOffer(videoObject.socketToReceive);

        }

        // once remote stream arrives, show it in the remote video element
        videoObject.pcToReceive.onaddstream = function(evt) {

            remoteView.srcObject = evt.stream;
        };

        videoObject.pcToReceive.ontrack = function(evt) {
            console.log("ontrack.");
            if (evt.track.kind === "video") {
                remoteView.srcObject = evt.streams[0];

                // document.getElementById('LoadingCanvasRemote').style.visibility = 'hidden';
            }

        };

        mode.onAir = true;

        videoObject.socketToReceive.send(JSON.stringify({ "mode": mode }));
    }


    videoObject.socketToReceive.onmessage = function(evt) {
        console.log('on message from Receiver server .');
        var message = JSON.parse(evt.data);
        console.log(message);

        if (message.type === 'answer') {
            videoObject.pcToReceive.setRemoteDescription(new RTCSessionDescription(message),
                () => {
                    videoObject.socketToReceive.send(JSON.stringify({ type: 'join' }));
                },
                logError);
        } else if (message.type === 'offer') {
            videoObject.pcToReceive.setRemoteDescription(new RTCSessionDescription(message),
                () => {
                    createAnswer(videoObject.socketToReceive, videoObject.pcToReceive);
                },
                logError);
        } else {
            // ICE candidate.
            videoObject.pcToReceive.addIceCandidate(new RTCIceCandidate(message.candidate), () => {}, logError);
        }
    };
    videoObject.socketToReceive.onclose = function() {
        console.log('closed socketToReceive by server.');
        if (mediaStream) mediaStream.stop();
        if (videoObject.pcToReceive != null) videoObject.pcToReceive.close();
        videoObject.pcToReceive = null;
    };
    videoObject.socketToReceive.onerror = function(e) {
        if (mediaStream) mediaStream.stop();
        if (videoObject.pcToReceive != null) videoObject.pcToReceive.close();
        videoObject.pcToReceive = null;
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


function switchMode(videoObject, target) {
    mode[target] = !mode[target];
    //socketToSend.send(JSON.stringify({"mode": mode}));
    videoObject.socketToReceive.send(JSON.stringify({ "mode": mode }));
}

function switchStart(videoObject) {
    var text, target = 'onAir';
    switchMode(videoObject, target);

}

function sleep_ms(millisecs) {
    var initiation = new Date().getTime();
    while ((new Date().getTime() - initiation) < millisecs);
}

function startMonitoring(videoMapObject) {
    front = cameras[0];
    var k = 0;
    var videoObject = null;

    for (k = 0; k < videoMapObject["sources"].length; k++) {
        videoObject = videoMapObject["sources"][k];
        GetConnectWebRTCServer(videoObject);
    }

    for (k = 0; k < videoMapObject["remotes"].length; k++) {
        videoObject = videoMapObject["remotes"][k];
        GetConnectWebRTCSender(videoObject);
        //switchStart(videoObject);
    }

}


//function switchButton(jsonFilename, videoId)
//{
//	var videoObj = document.getElementById('s'+ videoId),
//	 player = videoObj.parentNode.parentNode,
//	 buttonFlip = player.querySelector(".hipe-description .flip_a"),
//	 buttonStart = player.querySelector(".hipe-description .start_a")
//	 if (buttonStart.textContent == "Start")
//	 {
//		 if (mediaStream) mediaStream.stop();
//		 if (front == 0) {

//			front = cameras[0];
//			buttonFlip.textContent = "Rear";
//		 }
//		 sendJsonRequest(jsonFilename, videoId);
//		 buttonStart.textContent = "Stop";
//	 }
//	 else{
//		 if (mediaStream) mediaStream.stop();
//		 killTask(videoId);
//		 buttonStart.textContent = "Start";
//	 }
//}

function flipCamera(jsonFilename, videoId) {
    var videoObj = document.getElementById('s' + videoId),
        player = videoObj.parentNode.parentNode,
        buttonFlip = player.querySelector(".hipe-description .flip_a"),
        buttonStart = player.querySelector(".hipe-description .start_a");

    if (buttonFlip.textContent == "Rear") {
        if (mediaStream) mediaStream.stop();
        front = cameras[1];
        sendJsonRequest(jsonFilename, videoId);
        buttonFlip.textContent = "Front";
        buttonStart.textContent = "Stop";
    } else {
        if (mediaStream) mediaStream.stop();
        front = cameras[0];
        sendJsonRequest(jsonFilename, videoId);
        buttonFlip.textContent = "Rear";
        buttonStart.textContent = "Stop";
    }
}