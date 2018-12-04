
function startMonitoring()
{
	 if (pcToReceive != null) pcToReceive.close();
     pcToReceive = null;
     if (pcToSend != null) pcToSend.close();
     pcToReceive = null;
     socketToSend = null;
     socketToReceive = null;
	 
	document.getElementById('remoteView').style.visibility = 'visible';
    document.getElementById('LoadingCanvasRemote').style.visibility = 'visible';
    document.getElementById('LoadingCanvasSrc').style.visibility = 'visible';
    document.getElementById('selfView').style.visibility = 'visible';
	
	GetConnectWebRTCServer(target_port);
	GetConnectWebRTCSender(source_port);
	
}