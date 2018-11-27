var tabData = [];
var filtersTypes = {};
var nodes = [];
var countElement = 0;
var countItemDropped = 0;
var countDataInfo = 0;

var edge = [];
var activeNodeId = null;

$(function () {
    if ((sessionStorage.getItem('srvUrl') == undefined || sessionStorage.getItem('srvUrl') == undefined) && window.location.pathname != "/") {
        window.location.pathname = '/';
    }
});

var deleteModal = function (action, params) {
    return new Promise(
        function (resolve) {
            $('#confirmedDeleteModal').modal('toggle');
            activeNodeId = params;

            $('#confirmedDeleteBtn').unbind().on('click', function () {
                if (typeof isDataSource === "function" && isDataSource(nodes[activeNodeId].data.filter)) {
                    var split = activeNodeId.split('_');
                    var idInList = split[split.length - 1];

                    $('#button_collapse_' + idInList).click();

                    deleteNode(activeNodeId);
                    deleteItem($('#button_collapse_' + idInList), false);
                }

                if (action == 'node') {
                    deleteNode(activeNodeId);
                    cleanOptionBoard();
                }
                if (action == 'models') {
                    cleanTableLine(params);
                    notifySuccess('L\'élément a bien été supprimé');
                }


                $('#confirmedDeleteModal').modal('hide');

                resolve(params);
            });
        }
    );
}

var notifySuccess = function (msg) {
    $.notify({
        // options
        message: msg
    }, {
        // settings
        autoHide: false,
        type: 'success',
        animate: {
            enter: 'animated fadeInDown',
            exit: 'animated fadeOutUp'
        },
    });
};

var notifyError = function (msg) {
    if (msg == undefined || msg == null) {
        msg = "Une erreur est survenu. Veuillez vérifier vos informations."
    }

    $.notify({
        // options
        message: msg
    }, {
        // settings
        autoHide: false,
        type: 'danger',
        animate: {
            enter: 'animated fadeInDown',
            exit: 'animated fadeOutUp'
        },
    });
};

function cytoscape2json(needRoot, actionData) {
     var filters = [];
    document.title = 'Hipe | ' + $('#graphicName').val();

    var graph = {
        name: $('#graphicName').val(),
        filters: []
    };

    var newNodesSorted = sortNodes();

    newNodesSorted.forEach(function(nodesSorted, index) {

        Object.keys(nodesSorted).forEach(function(keyInSorted) {

            var key = nodesSorted[keyInSorted]['key'];

            var filtername = nodes[key].data.filter;
            var filter = {};
            filter[filtername] = [];
            filter[filtername] = {};
            filter[filtername].name = nodes[key].data.name;

            if (nodesSorted[keyInSorted]['parents'] != undefined && nodesSorted[keyInSorted]['parents'].length > 0) {
                var nodeExist = false;
                var isRootExist = false;
                var listOfNeed = [];

                nodesSorted[keyInSorted]['parents'].forEach(function(parentsEl) {
                    Object.keys(nodesSorted).forEach(function(node) {
                        if (nodesSorted[node].data.name == parentsEl) {
                            if (nodesSorted[node].data.filter == "RootFilter") {
                                isRootExist = true;
                            }
                            nodeExist = true;
                        }
                    });

                    if (!nodeExist) {
                        delete nodesSorted[keyInSorted]['parents'][parentsEl];
                    }
                });

                filter[filtername].need = nodesSorted[keyInSorted]['parents'];
            }

            if (nodes[key].data != undefined && nodes[key].data != '') {
                for (var param in nodes[key].data) {
                    if (param != "id" && param != "filter" && nodes[key].data[param] != "") {
                        filter[filtername][param] = nodes[key].data[param];
                    }
                }
            }

            if (!isDataSource(nodes[key].data.filter)) {
                filters.push(filter);
            }

        });
    });


    graph.filters = filters;
    graph.orchestrator = $('#orchestrator').val();

    var types = $('.data-type').toArray();

    if (types.length > 0) {
        graph.data = {};
        graph.data.datasource = [];

        //var types = $('.data-type').toArray();
        var typesOrder = [];
        var typesSorted = [];

        types.forEach(function (type) {
            var datas = $(type).find('input[id^="attr_"]').toArray();
            var index = 0;

            if (datas.length > 0) {
                datas.forEach(function (el) {
                    var i = el.id.split("_")[1];
                    if (i == parseInt(i, 10)) {
                        var attr = $('#attr_' + i + '_' + index).val();
                        var config = $('#config_' + i + '_' + index).val();

                        if (attr == 'name') {
                            var filter = getFilterByName(config);

                            if (isDataSource(filter)) {
                                Object.keys(newNodesSorted).forEach(function (position) {
                                    Object.keys(newNodesSorted[position]).forEach(function (key) {
                                        var filter = getFilterByName(config);

                                        if (newNodesSorted[position][key].key == filter) {
                                            var num = 0;
                                            if(typesOrder[position] != undefined){
                                                num = typesOrder[position].length + 1;
                                            }
                                            else{
                                                typesOrder[position] = [];
                                            }

                                            typesOrder[position][num] = type;
                                        }
                                    })
                                })
                            }
                        }

                        index++;
                    }
                });
            }
        });


        typesOrder.forEach(function (typeByPosition) {
            typeByPosition.forEach(function (type) {
                var element = {};

                var dataType = $(type).attr('type');
                element[dataType] = {};

                var datas = $(type).find('input[id^="attr_"]').toArray();
                var index = 0;

                if (datas.length > 0) {
                    datas.forEach(function (el) {
                        var i = el.id.split("_")[1];


                        if (i == parseInt(i, 10)) {
                            var attr = $('#attr_' + i + '_' + index).val();
                            var config = $('#config_' + i + '_' + index).val();

                            if (attr == 'name') {
                                var filter = getFilterByName(config);

                                if (nodes[filter].data.need && Array.isArray(nodes[filter].data.need)) {
                                    element[dataType]['need'] = nodes[filter].data.need;
                                }
                            }

                            if (attr != '' && config != '') {
                                if (config == 'true') {
                                    config = true;
                                }
                                if (config == 'false') {
                                    config = false;
                                }
                                element[dataType][attr] = config;

                                /*var configObj = $.parseJSON(datas[dataType]);
                                Object.keys(configObj).forEach(function (singleConfig) {
                                    if(actionData) {
                                        if (singleConfig == 'array') {
                                            configObj[singleConfig].forEach(function (data) {
                                                var indexBase64 = 0;
                                                if (data.type == "IMGB64") {
                                                    data.data = imgsB64[0];
                                                    indexBase64++;
                                                }
                                            })
                                        }
                                        else if (element[dataType].type == "IMGB64" && singleConfig == 'data') {
                                            configObj[singleConfig] = imgsB64[0];
                                        }
                                    }
                                    //element[dataType].datasource[singleConfig] = configObj[singleConfig];
                                });*/
                            }

                            index++;
                        }
                    })
                }

                graph.data.datasource.push(element);
            });
        });

    }
    return graph;
}

function generateJson(needRoot, actionData) {

    var graph = cytoscape2json(needRoot, actionData);

       return JSON.stringify(graph, null, "\t");
}


function sortNodes() {

    var newNodesSorted = [];
    var allOrdered = false;
    var elementAdded = 0;
    var nodesCopy = $.extend({}, nodes);
    var matchTable = [];

    if (Object.keys(nodesCopy).length == 0) {
        allOrdered = true;
    }

    for (var i = 0; allOrdered == false; i++) {
        Object.keys(nodesCopy).forEach(function (key) {
            var parents = [];
            var isPassed = false;
            var countEdge = 0;

            var notAlreadyAdd = false;

            if (nodesCopy[key] != undefined) {
                if (newNodesSorted[i] == undefined) {
                    newNodesSorted[i] = {};
                }

                if (i == 0 && nodesCopy[key] != undefined && nodesCopy[key].data.need == undefined) {
                    newNodesSorted[i][nodesCopy[key].data.id] = {};

                    newNodesSorted[i][nodesCopy[key].data.id]['key'] = key;
                    matchTable[nodes[key].data.name] = key;
                    //delete nodesCopy[key];

                    elementAdded++;
                }

                if (nodesCopy[key] != undefined && nodesCopy[key].data.need != undefined) {
                    var rootIndex = 0;
                    nodesCopy[key].data.need.forEach(function (parentsEl) {
                        if (matchTable[parentsEl] == undefined) {
                            var nodeExist = false;
                            var isRootExist = false;
                            var listOfNeed = [];

                            Object.keys(nodes).forEach(function (node) {
                                if (nodes[node].data.name == parentsEl) {
                                    if (nodes[node].data.filter == "RootFilter") {
                                        isRootExist = true;
                                    }
                                    nodeExist = true;

                                    if (listOfNeed.indexOf(nodes[node].data.name == -1)) {
                                        listOfNeed.push(nodes[node].data.name);
                                    }
                                    else {
                                        delete nodesCopy[key].data.need[listOfNeed.indexOf(nodes[node].data.name)];
                                    }
                                }
                            });

                            if (nodeExist) {
                                notAlreadyAdd = true
                            }
                            else {
                                var index = nodesCopy[key].data.need.indexOf(parentsEl);
                                nodesCopy[key].data.need.splice(index, 1);
                            }

                        }
                        if (matchTable[parentsEl] > rootIndex) {
                            rootIndex = matchTable[parentsEl];
                        }
                    })

                    if (!notAlreadyAdd) {
                        if (newNodesSorted[rootIndex + 1] == undefined) {
                            newNodesSorted[rootIndex + 1] = {};
                        }
                        newNodesSorted[rootIndex + 1][nodesCopy[key].data.id] = {};

                        newNodesSorted[rootIndex + 1][nodesCopy[key].data.id]['key'] = key;
                        newNodesSorted[rootIndex + 1][nodesCopy[key].data.id]['parents'] = parents;
                        matchTable[nodesCopy[key].data.name] = rootIndex + 1;
                        elementAdded++;

                        delete nodesCopy[key];
                    }
                }

                if (elementAdded == Object.keys(nodes).length || i >= Object.keys(nodesCopy).length) {
                    allOrdered = true;
                }
            }


        });
    }

    return newNodesSorted;
}

function AssignBackGroundImage(name, target) {

     
};

function addElementInGraph(name, helper, values, graphical, isCopy) {
    var box = $(".boardpanel").offset();
    
    if (helper != null) {
        var offset = helper.offset();
        var xPos = offset.left;
        var yPos = offset.top - 40;
    }

    var nameExist = 0;
    Object.keys(nodes).forEach(function (node) {
        if (nodes[node].data.filter == name) {
            nameExist++;
        }
    });

    var formatedName = name;
    if (nameExist > 0) {
        formatedName = name + '_' + countElement;
    }

    var newNode = {
        data: {
            name: formatedName,
        }
    };

    var position_x = xPos - box.left;
    var position_y = yPos - box.top;

    /*var http = new XMLHttpRequest();
    var url = "/point/" + model.id + "/" + formatedName;
    http.open("GET", url, true);

    http.setRequestHeader("Content-type", "application/json");

    http.onload = function () {
        var point = JSON.parse(this.response);

        if(point.length > 0) {
            point = point[0];

            position_x = point.position_x;
            position_y = point.position_y;
        }
    };

    http.send();
*/
    if (helper != null) {
        newNode['renderedPosition'] = {x: position_x, y: position_y};
    }

    newNode.data.filter = name;

    if (filtersTypes[name] != undefined && filtersTypes[name]["fields"] != undefined) {
        Object.keys(filtersTypes[name]["fields"]).forEach(function (key) {
            newNode.data[key] = filtersTypes[name]["fields"][key];
        });
    }

    if (values != null) {
        Object.keys(values).forEach(function (key) {
            newNode.data[key] = values[key];
        });
        if (isCopy != undefined && isCopy === true) {
            newNode.data["name"] = formatedName;
        }
    }

    nodes[name + '_' + countElement] = newNode;
    newNode.data.id = countElement;

    countElement++;

    if (graphical != null && graphical == true) {
        cy.add(newNode);
    }

    var nodeTarget = cy.filter('node[id = "' + newNode.data.id + '"]');
    if (isDataSource(name)) {
        nodeTarget[0].style({'shape': 'ellipse'});

        var split = name.split('DataSource');
        nodeTarget[0].style({ 'content': split[0] });
        AssignBackGroundImage(name, nodeTarget);
    }

    $('#itemDroped' + countItemDropped).css('top', '-' + $('#svgglobal').css('height'));

    return name + '_' + (countElement - 1);
}

var videoMap = new Map();
var remoteLog = null;

function insertText(cm, data) {
   
    //var cm = $(".CodeMirror")[0].CodeMirror;
    var doc = cm.getDoc();
    var cursor = doc.getCursor(); // gets the line number in the cursor position
    var line = doc.getLine(cursor.line); // get the line contents
    var pos = {
        line: cursor.line
    };
    if (line.length === 0) {
        // check if the line is empty
        // add the data
        doc.replaceRange(data, pos);
    } else {
        // add a new line and the data
        doc.replaceRange("\n" + data, pos);
    }
    cm.scrollIntoView({ line: cm.lineCount() - 1, char: 5 }, 200);
}
var remoteLog = {
    port : 9136,
    logAreaId: "logArea",
    socketToReceive: null
}

function initBaseLogService() {
    var myTextarea = document.getElementById("logArea");
    myTextarea.value = "";
    if (myTextarea.parentNode.childElementCount <= 1) {
        var editor = CodeMirror.fromTextArea(myTextarea,
            {
                lineNumbers: true,
                matchBrackets: true,
                styleActiveLine: true,
                //mode: "python",
                //theme: "solarized dark"
            });
        //editor.setOption("mode", "python");
    }

    if (remoteLog.socketToReceive != null) {
        return;
    }

    var cm = $('#logArea').parent().find('.CodeMirror')[0].CodeMirror;
    cm.setValue("");
    cm.clearHistory();
    var graph = cytoscape2json(true, true);
    var filters = graph['filters'];
    var logArea = $('#logArea');
    var textBuffer = [];

    var regex = /\bhttps?:\/\/(\S+):[0-9]+/gi;

    var matches = regex.exec(sessionStorage.getItem('srvUrl'));

   
    remoteLog.socketToReceive = new WebSocket('wss://' + matches[1] + ":" + remoteLog.port);
    remoteLog.socketToReceive.onopen = function() {
        console.log('remoteLog open');
                
    }
    remoteLog.socketToReceive.onmessage = function(event) {
        //console.log('remoteLog message : ' + event.data);
        textBuffer.push(event.data);
                
    }

    remoteLog.socketToReceive.onclose = function(event) {
        //console.log('remoteLog message : ' + event.data);
        remoteLog.socketToReceive = null;

    }
    remoteLog.socketToReceive.onerror = function(event) {
        console.log('remoteLog error during connexion : ');
        remoteLog.socketToReceive = null;
    }
    var intervalBaseID = setInterval(function() {
        if (textBuffer.length > 0) {
            insertText(cm, textBuffer.join('\n'));
        }

        textBuffer = [];
    }, 5000);
}

function initLogService() {
    var myTextarea = document.getElementById("logArea");
    myTextarea.value = "";
    if (myTextarea.parentNode.childElementCount <= 1) {
        var editor = CodeMirror.fromTextArea(myTextarea,
            {
                lineNumbers: true,
                matchBrackets: true,
                styleActiveLine: true,
                //mode: "python",
                //theme: "solarized dark"
            });
        //editor.setOption("mode", "python");
    }
    var cm = $('#logArea').parent().find('.CodeMirror')[0].CodeMirror;
    cm.setValue("");
    cm.clearHistory();
    var graph = cytoscape2json(true, true);
    var filters = graph['filters'];
    var logArea = $('#logArea');
    var textBuffer = [];

    for (var i = 0; i < filters.length; i++)
    {
        if (filters[i].ForwardLogToWeb != null) {
            remoteLog = {
                port : filters[i]['ForwardLogToWeb'].port,
                logAreaId: "logArea",
                socketToReceive: null,
            }
            remoteLog.socketToReceive = new WebSocket('wss://' + window.location.hostname + ":" + remoteLog.port);
            remoteLog.socketToReceive.onopen = function() {
                console.log('remoteLog open');
                
            }
            remoteLog.socketToReceive.onmessage = function(event) {
                //console.log('remoteLog message : ' + event.data);
                textBuffer.push(event.data);
                
            }
            break;
        }
    }
    var intervalID = setInterval(function() {
        if (textBuffer.length > 0) {
            insertText(cm, textBuffer.join('\n'));
        }

        textBuffer = [];
    }, 5000);
}

function initMonitor() {
    videoMap["sources"] = [];
    videoMap["remotes"] = [];
    var graph = cytoscape2json(true, true);
    var dataSource = graph['data']['datasource'];
    for (var i = 0; i < dataSource.length; i++)
    {
        if (dataSource[i].WebRTCVideoDataSource != null) {
            var source = {
                port: dataSource[i]['WebRTCVideoDataSource'].port,
                videoId: "sView_" + i,
                socketToSend: null,
                pcToSend: null
            };
            videoMap["sources"].push(source);
                
        }
    }
    var filters = graph['filters'];
    for (var i = 0; i < filters.length; i++)
    {
        if (filters[i].WebRTCSender != null) {
            var remote = {
                port : filters[i]['WebRTCSender'].port,
                videoId: "rView_" + i,
                socketToReceive: null,
                pcToReceive: null
            }

            videoMap["remotes"].push(remote);
        }
    }

    //Create Video tag in html
    var sourceVideos = $('#sourceVideos');
    var remoteVideos = $('#remoteVideos');
    var k = 0;
    var videoObject = null;
    sourceVideos.html('');
    var htmlCode = "";
    if (videoMap["sources"].length === 0) {
        $('#sourceVideos').css("display", "none");
    } else {
        $('#sourceVideos').css("display", "block");
    }
    if (videoMap["remotes"].length === 0) {
        $('#remoteVideos').css("display", "none");
    } else {
        $('#remoteVideos').css("display", "block");
    }


    for (k = 0; k < videoMap["sources"].length; k++) {
        videoObject = videoMap["sources"][k];
        htmlCode += "<video id='" + videoObject.videoId + "' class='videoBox' autoplay controls></video>";
    }
    sourceVideos.html(htmlCode);
    remoteVideos.html('');
    htmlCode = "";
    for (k = 0; k < videoMap["remotes"].length; k++) {
        videoObject = videoMap["remotes"][k];
        htmlCode += "<video id='" + videoObject.videoId + "' class='videoBox' autoplay controls></video>";
    }
    remoteVideos.html(htmlCode);

    window.startMonitoring(videoMap);
}

function showMonitor() {
    if ($('#Monitor').css("display") !== "none") {
        $('#Monitor').css("display", "none");
    } else {
     
        $('#Monitor').css("display", "block");
       

        //<video id="rView" class="videoBox" controls></video>

        
    }

}

function showLog() {
    if ($('#Log').css("visibility") === "visible") {
        $('#Log').css("visibility", "hidden");
    } else {
        $('#Log').css("visibility", "visible");
        
    }


}

function showJson() {
    if ($('#Json').css("visibility") === "visible") {
        $('#Json').css("visibility", "hidden");
    } else {
        $('#Json').css("visibility", "visible");
    }

}

