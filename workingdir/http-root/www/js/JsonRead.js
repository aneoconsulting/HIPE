

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
