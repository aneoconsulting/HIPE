var model = null;
var imgsB64 = [];
var commands = [];
var restriction = false;
var oldNodes = [];

var WindowsSize=function() {
    var h = $(window).height();
    var menubarHeight = $('#menu-header').outerHeight();
    var toolbarHeight = $('#menu-toolbar').outerHeight();
    var panelHeadingHeight = $('.panel-heading').outerHeight();
    var monitorHeaderHeight = $('#Monitor-header').outerHeight();
    var monitorHeaderHeight = $('#Json-header').outerHeight();
    var searchFilterHeight = $('#searchFilter').outerHeight();

    var boardHeight = h - menubarHeight - toolbarHeight - 5;
    $('.boardpanel').height(boardHeight);
    $('#panelFilter').height(boardHeight);
    $('#panelBodyFilter').height(boardHeight - panelHeadingHeight - searchFilterHeight - 5);
    $('#board').height(boardHeight - panelHeadingHeight);
    $('#Editor').height(boardHeight - panelHeadingHeight - 35);
    $('#Editor-area').height(boardHeight - panelHeadingHeight - 35);
    $('.EditorPanel').height(boardHeight - panelHeadingHeight);
    $('.CodeMirror').height(boardHeight - panelHeadingHeight - 35);
    $('.optionspanel').css('max-height', boardHeight - panelHeadingHeight - searchFilterHeight + 'px');

    $('.monitorpanel').css('min-height', $('#Monitor').height() + "px");
    $('#Monitor-area').css('min-height', $('#Monitor').height() - monitorHeaderHeight - 12 + "px");

    $('.Logpanel').height($('#Log').height() + "px");
    $('#Log-area').height($('#Log').height() - monitorHeaderHeight  - 43 + "px");
    $('.Jsonpanel').height($('#Json').height() + "px");
    $('#Json-area').height($('#Json').height() - monitorHeaderHeight  - 43 + "px");

};
//var initSize=function() {
//    $('#Monitor').height("480px");
//    ('#Monitor-area').height("480px");
//    WindowsSize();
//}


$(document).ready(WindowsSize); 
$(window).resize(WindowsSize); 


function ApplyImageToNode(type, target) {
    if (! isDataSource(type)) {
        target[0].style({ 'shape': 'rectangle' });
        //nodeTarget[0].style({ 'border-color': 'red' });

        //nodeTarget[0].style({ 'background-image': 'url(/images/logo-hipe.png)' });

        target[0].style({ 'background-fit': 'cover cover' });
    }
    else {
        target[0].style({ 'shape': 'ellipse' });
        //target[0].style({ 'border-color': 'lightgrey' });
        //nodeTarget[0].style({ 'background-image':  'url(http://icons.iconarchive.com/icons/graphicloads/100-flat/256/home-icon.png)' });
    }

    if (new RegExp('WebRTC.*DataSource', 'g').test(type)) {
        target[0].style({ 'width': '60px' });
        target[0].style({ 'height': '60px' });
        target[0].style({ 'background-image': 'url(/images/webrtcDataSource.jpg)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('VideoDataSource', 'g').test(type)) {
        target[0].style({ 'width': '60px' });
        target[0].style({ 'height': '60px' });
        target[0].style({ 'background-image': 'url(/images/VideoDataSource-2.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('Resize', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/resize.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('Python.*', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/python.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('Concat.*', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/concat.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('.*Sender.*', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/video-emit.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    } 
    else if (new RegExp('.*Detect.*', 'g').test(type)) {
        target[0].style({ 'shape': 'ellipse' });
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/objectdetection.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('Text.*', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/textlog.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }
    else if (new RegExp('Show.*', 'g').test(type)) {
        target[0].style({ 'width': '50px' });
        target[0].style({ 'height': '50px' });
        target[0].style({ 'background-image': 'url(/images/showvideo.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });

    }
    else {
        //target[0].style({ 'shape': 'ellipse' });
        target[0].style({ 'width': '70px' });
        target[0].style({ 'height': '55px' });
        target[0].style({ 'background-image': 'url(/images/cpp.png)' });
        target[0].style({ 'background-fit': 'cover cover' });
        //target[0].style({ 'border-color': 'lightgrey' });
    }

};

function keyEventEditor(e) {
    if (e.ctrlKey && e.keyCode === 83) {
        e.preventDefault();
        e.stopPropagation();
        // Save
        saveCode();

        return false;
    }
};


function saveModelEvent(e) {
    if (e.ctrlKey && e.keyCode === 83) {
        e.preventDefault();
        e.stopPropagation();
        // Save
        saveModel('#saveModel');

        return false;
    }
    if (e.ctrlKey && e.keyCode == 69) {
        e.preventDefault();
        e.stopPropagation();
        // execute
        sendDataToServer();

        return false;
    }
    if (e.ctrlKey && e.keyCode === 68) {
        e.preventDefault();
        e.stopPropagation();
        var selectedNodes = cy.$('node:selected');
        var nodes = cy.$('node');

        for (var activeNodeId = 0; activeNodeId < selectedNodes.length; activeNodeId++) {
            var obj = selectedNodes[activeNodeId].data();
            var element = window.addElementInGraph(obj.filter, null, obj, true, true);
            displayOptions(element);
            var nodeTarget = cy.filter(function(el, i){
                if( el.isNode() && el.data("name") === element ){
                    return true;
                }
                return false;
            });
            //Remove dependencies coming from the source of copy
            nodeTarget.data().need = [];

            ApplyImageToNode(nodeTarget.data().filter, nodeTarget);
        }
    }
};

$(function () {
    window.initBaseLogService();
    document.getElementById("linkDemo").href = sessionStorage.getItem('srvUrl');
   
    var cy = window.cy = cytoscape({
        container: document.getElementById('board'),

        boxSelectionEnabled: true,
        selectionType: 'additive',
        autounselectify: false,
        userZoomingEnabled: true,
        wheelSensitivity: 0.1,
        ready: function () {
            console.log("Graph is ready");
        },

        layout: {
            name: 'klay',

            directed: true
        },

        style: [
            {
                selector: 'node',
                style: {
                    'content': 'data(filter)',
                    'text-opacity': 0.5,
                    'text-valign': 'top',
                    'text-halign': 'center',
                    'background-color': '#fff',
                    'shape': 'rectangle',
                    'width': '90',
                    'height': '60',
                    'border-width': '1px',
                    'border-color': 'grey',
                    'text-margin-x': '0px',
                    'text-margin-y': '0px'
                }
            },
            {
                selector: '.node-selected',
                css: {
                    'border-width': '3px',
                    'border-color': 'red'
                  
                }
            },
            {
                selector: '.edge-hover',
                css: {
                    'content': "X",
                    'font-size': '25px',
                    'font-weight': 'bold',
                    'color': "red"
                }
            },
            {
                selector: '.edgehandles-ghost-edge',
                css: {
                    'padding-top': '20px!important'
                }
            },
            {
                selector: 'edge',
                style: {
                    'curve-style': 'unbundled-bezier',
                    'width': 5,
                    'target-arrow-shape': 'triangle',
                    'line-color': '#9dbaea',
                    'target-arrow-color': '#9dbaea'
                }
            }
        ],

        elements: {
            nodes: nodes,
            edges: edge
        },

       

    });

    var board = document.getElementById('board');
    var keyCtrlDown = false;

    $('#trashBtn').on('click',
        function(event) {
            console.log("Deleting node " + cy.$('node:selected'));
            var nodes = cy.$('node:selected');
            for(var activeNodeId = 0; activeNodeId < nodes.length; activeNodeId++) {
                deleteNode(nodes[activeNodeId].data().filter + "_" + nodes[activeNodeId].data().id);
            }
            cleanOptionBoard();
        });
   
    cy.edgehandles({
        toggleOffOnLeave: true,
        handleNodes: "node",
        handleSize: 10,
        handleColor: '#ADADAD',
        handlePosition: 'middle bottom',
        edgeType: function () {
            return 'flat';
        },
        complete: function (sourceNode, targetNodes, addedEntities) {
            if (addedEntities.length > 0) {
                cy.remove(addedEntities);

                var targetName = targetNodes.data().filter;
                var targetId = targetNodes.data().id;
                var selector = targetName + "_" + targetId;

                var sourceName = sourceNode.data().filter;
                var sourceId = sourceNode.data().id;
                var sourceSelector = sourceName + "_" + sourceId;

                var nodes = [];
                nodes.push(sourceSelector);

                if (targetNodes.data().need == undefined) {
                    targetNodes.data().need = [];
                }

                var nodeAlreadyExist = false;

                targetNodes.data().need.forEach(function (parentEl) {
                    if (parentEl == sourceNode.data().name) {
                        nodeAlreadyExist = true;
                    }
                });

                if (!nodeAlreadyExist) {
                    targetNodes.data().need.push(sourceNode.data().name)

                    targetNodes.connectedEdges().forEach(function (edge) {
                        var sourceName = edge.source().data().filter;
                        var sourceId = edge.source().data().id;
                        var selectorNodeEdge = sourceName + "_" + sourceId;

                        if (sourceNode.data().id != edge.source().data().id && selector != selectorNodeEdge) {
                            nodes.push(selectorNodeEdge);
                        }
                    });

                    drawConnections(selector, false, false, nodes);
                }
               
            }
        }
    });

    cy.panzoom({
        zoomOnly: true,
        zoomInIcon: 'glyphicon-plus',
        zoomOutIcon: 'glyphicon-minus',
        resetIcon: 'glyphicon glyphicon-resize-small'
    });

    cy.on('cyedgehandles.showhandle', 'node', function (e) {
        var srcNode = this;

        $('canvas')[0].getContext('2d');

        srcNode.addClass('add-marge');

    });

    cy.on('mouseover', 'edge', function (evt) {
        var edgeTarget = evt.target;
        edgeTarget.addClass('edge-hover');

        edgeTarget.on('click',
            function() {
                edge.forEach(function(singleEdge, key) {
                    var idSource = edgeTarget[0].source().id();
                    var idTarget = edgeTarget[0].target().id();

                    if (singleEdge.data.source == idSource || singleEdge.data.target == idTarget) {
                        edge.splice(key, 1);
                        var nodeParentName = edgeTarget[0].source().data().name;
                        edgeTarget[0].target().data().need.forEach(function(element, key) {
                            if (element == nodeParentName) {
                                edgeTarget[0].target().data().need.splice(key, 1);
                            }
                        });
                    }
                });
                cy.remove(edgeTarget);
            });
    });

    cy.on('mouseout', 'edge', function (evt) {
        var edge = evt.target;
        edge.removeClass('edge-hover');
    });

    var doubleClickDelayMs = 350;
    var previousTapStamp = 0;

    cy.on('boxend', function(event) {
        console.log("Godd bye box");
        $("#bodyBoard").focus();
    });
    cy.on('doubleTap',
        'node',
        function(event, originalTapEvent) {
            if ($('#Editor').css("visibility") === "visible") {
                $('#Editor').css("visibility", "hidden");
            } else {
                if (event.target.data().filter === "PythonCode") {
                    document.removeEventListener('keydown', saveModelEvent);
                    document.addEventListener('keydown', keyEventEditor);

                    $('#Editor').css("visibility", "visible");
                    var myTextarea = document.getElementById("codeArea");
                    if (myTextarea.parentNode.childElementCount <= 2) {
                        var editor = CodeMirror.fromTextArea(myTextarea,
                            {
                                lineNumbers: true,
                                matchBrackets: true,
                                styleActiveLine: true,
                                mode: "python",
                                theme: "solarized dark"
                            });
                        editor.setOption("mode", "python");
                    }
                    $('#Editor').data("target", event.target.data());
                    var editor = $('#codeArea').parent().find('.CodeMirror')[0].CodeMirror;
                    editor.clearHistory();
                    editor.setValue(event.target.data().code_impl);

                }

            }
        });

    cy.on('tap', 'node', function (evt) {
        if (!keyCtrlDown) {
            cy.$('node:selected').unselect();
            var currentTapStamp = evt.timeStamp;
            var msFromLastTap = currentTapStamp - previousTapStamp;

            if (msFromLastTap < doubleClickDelayMs) {
                evt.target.trigger('doubleTap', 'node', evt);
            } else {
                var node = this;
                displayOptions(this.data().filter + '_' + this.data().id);
            }
            previousTapStamp = currentTapStamp;
        }


    });

    cy.on('select',
        'node',
        function(event) {
            // cyTarget holds a reference to the originator
            // of the event (core or element)
            if (keyCtrlDown) {
                event.target.addClass('node-selected');
               
                
            } else {
                event.target.unselect();
            }
            
        });

    cy.on('unselect',
        'node',
        function(event) {
            // cyTarget holds a reference to the originator
            // of the event (core or element)
            event.target.removeClass('node-selected');
            

        });

    $("#bodyBoard").focus();

    cy.on('tap',
        function(event) {
            // cyTarget holds a reference to the originator
            // of the event (core or element)
            if (event.target === cy) {
                if (!keyCtrlDown)
                    cy.$('node:selected').unselect();
                $('#optionspanel').css("display", "none");
            }
            $("#bodyBoard").focus();
        });


    //$("#bodyBoard").focusin(function(event) {
    //    console.log("onfucus on board");
    //});
    //$("#bodyBoard").focusout(function(event) {
    //    console.log("focus lost on board");
    //});

    $("#bodyBoard").keydown(function(event) {
       
        if (event.keyCode === 17) {
            event.preventDefault();
            keyCtrlDown = true;
            $( '.nodeToolbar' ).css( "visibility", "visible" );
        }
    });

    $("#bodyBoard").keyup(function(ev){
       
        if (ev.keyCode === 17 && keyCtrlDown) {
            //event.preventDefault();
            keyCtrlDown = false;
         
            $( '.nodeToolbar' ).css( "visibility", "hidden" );
        }
    });
  
    

    $("#board").droppable({
        accept: ".list-group-item",
        classes: {
            "ui-droppable-active": "ui-state-active",
            "ui-droppable-hover": "ui-state-hover"
        },
        drop: function (event, ui) {
            $(ui.draggable).draggable({revert: false});

            var element = addElementInGraph($(ui.draggable).attr('data-name'), $(ui.helper), null, true);
            displayOptions(element);

            var nodeTarget = cy.filter('node[id = "' + nodes[element].data.id + '"]');

            ApplyImageToNode(nodes[element].data.filter, nodeTarget);

         
            var type = $(ui.draggable).attr('id');

            var split = element.split("_");
            var idNode = split[split.length - 1];

            addConfiguration(idNode);

            var currentId = countDataInfo - 1;

            $("#delete_config_" + (idNode)).attr('selector', element);
            $('#attr_' + (idNode)).val(type);
            $('#button_collapse_' + (idNode)).html('<span class="glyphicon glyphicon-chevron-down"></span> ' +
                type +
                ' (' +
                nodes[element].data.name +
                ')');

            addDataConfiguration(type, nodes[element].data.name, idNode);
            //}

            $("#searchFilter").focus();
            $("#searchFilter").select();
        }
    });

    var groupsFilterBodyRaw = {
        "name": "filters",

        "command":
            {
                "type": "GroupFilters"
            }
    };

    var insertFilter = function (i, value) {
        $('#filtersType').append(
            '<a id="' + i + '" href="javascript:void(0)" data-name="' + i + '" class="list-group-item ui-draggable ui-draggable-handle">  ' +
            '<i class="fa fa-comment fa-fw"></i> ' + i + '</a>'
        );

        $('#' + i).draggable({
            revert: true, helper: "clone", cursorAt: {top: 20, left: 30},
            start: function (e, ui) {
                var style = {
                    'content': 'data(id)',
                    'text-opacity': 0.5,
                    'text-valign': 'center',
                    'text-halign': 'right',
                    'background-color': '#fff',
                    'shape': 'rectangle',
                    'width': '90',
                    'height': '60'
                };

                $(ui.helper).css(style);
            }
        });
    };
    
   
    $.ajax({
        data: JSON.stringify(groupsFilterBodyRaw),
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La récupération des types de filtres n\'a pas pu être effectuée.')
        },
        success: function (data) {
            var filtersTypes = data;

            var newFilters = [];
            var index = 0;


            $.each(filtersTypes, function (typeFilter, listTypeFilters) {
                var haveSubType = false;

                var content = '';
                if(index != 0){
                    content = content + "<hr/>";
                }

                if($('#accordion' + index).length <= 0) {
                    content = content + '' +
                        '<div class="panel-group" id="accordion' + index + '" role="tablist" aria-multiselectable="true">\n' +
                        '  <div class="panel panel-default">\n' +
                        '    <div class="panel-heading" role="tab" id="headingOne">\n' +
                        '      <h3 class="panel-title">\n' +
                        '        <a role="button" data-toggle="collapse" data-parent="#accordion' + index + '" href="#collapse' + index + '" aria-expanded="true" aria-controls="collapseOne">\n' +
                        '          <span class="glyphicon glyphicon-chevron-down"></span>' + typeFilter + '\n' +
                        '        </a>\n' +
                        '      </h3>\n' +
                        '    </div>\n' +
                        '    <div id="collapse' + index + '" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingOne">\n' +
                        '      <div class="panel-body" id="' + typeFilter + index + '">\n' +
                        '      </div>\n' +
                        '    </div>\n' +
                        '  </div>\n' +
                        '</div>'


                    $('#filtersType').append(
                        content
                    );


                }

                index++;
                var subIndex = 0;


                $.each(listTypeFilters, function (subTypeFilter, listSubTypeFilter) {

                    if(!Number.isInteger(subTypeFilter)) {
                        haveSubType = true;

                        if($('#sousaccordion' + (index - 1) + '_' + subIndex).length <= 0) {
                            var subContent = '' +
                                '       <div class="panel-group" id="sousaccordion' + (index - 1) + '_' + subIndex + '" role="tablist" aria-multiselectable="true">\n' +
                                '  <div class="panel panel-default">\n' +
                                '    <div class="panel-heading" role="tab" id="headingOne">\n' +
                                '      <h4 class="panel-title">\n' +
                                '        <a role="button" data-toggle="collapse" data-parent="#sousaccordion' + (index - 1) + '_' + subIndex + '" href="#subcollapse' + (index - 1) + '_' + subIndex + '" aria-expanded="true" aria-controls="collapseOne">\n' +
                                '          <span class="glyphicon glyphicon-chevron-down"></span>' + subTypeFilter + '\n' +
                                '        </a>\n' +
                                '      </h4>\n' +
                                '    </div>\n' +
                                '    <div id="subcollapse' + (index - 1) + '_' + subIndex + '" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingOne">\n' +
                                '      <div class="panel-body" id="sub' + subTypeFilter + (index - 1) + '_' + subIndex + '">\n' +
                                '      </div>\n' +
                                '    </div>\n' +
                                '  </div>\n' +
                                '</div>';

                            $('#' + typeFilter + (index - 1)).append(subContent);

                            subIndex++;
                        }
                    }



                    $.each(listSubTypeFilter, function (i, value) {

                        if(Number.isInteger(i)) {
                            i = Object.keys(value)[0];
                            value = value[i];
                        }

                        var selector = typeFilter + (index - 1);

                        if(haveSubType){
                            selector = 'sub' + subTypeFilter + (index - 1) + '_' + (subIndex - 1)
                        }

                        $('#' + selector).append(
                            '<a id="' + i + '" href="javascript:void(0)" data-name="' + i + '" class="list-group-item ui-draggable ui-draggable-handle">  ' +
                            '<i class="fa fa-comment fa-fw"></i> ' + i + '</a>'
                        );

                        $('#' + i).draggable({
                            revert: true, helper: "clone", cursorAt: {top: 20, left: 30},
                            start: function (e, ui) {
                                var style = {
                                    'content': 'data(id)',
                                    'text-opacity': 0.5,
                                    'text-valign': 'center',
                                    'text-halign': 'right',
                                    'background-color': '#fff',
                                    'shape': 'rectangle',
                                    'width': 'auto',
                                    'height': '40',
                                }

                                $(ui.helper).css(style);
                            }
                        });
                    });
                });
            });
        }

    });

    var filterBodyRaw = {
        "name": "filters",

        "command":
            {
                "type": "Filters"
            }
    };

    $.ajax({
        data: JSON.stringify(filterBodyRaw),
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La récupération des types de filtres n\'a pas pu être effectuée.')
        },
        success: function (data) {
            filtersTypes = data;

            if ($('#modelObj').attr('data') != null && $('#modelObj').attr('data') != "") {
                model = $('#modelObj').attr('data');

                model = JSON.parse(model)[0];

                setShema(model.json, true);

                if (model.id_of_next_element != null && model.id_of_next_element != 0) {
                    countElement = model.id_of_next_element;
                }
                else {
                    countElement = model.json.filters.length;
                }

                $('#graphicName').val(model.name);

            }
            window.draggable("#Monitor");
            window.draggable(".nodeToolBar");
            window.draggableAndresizable(".videoBox");
            
            window.draggableAndresizable("#optionspanel");
            window.draggableAndresizable("#Log");
            window.draggableAndresizable("#Editor"); 
            window.draggableAndresizable("#Json");
        }


    });
    var wto;
    var firstFullRendering = true;
    cy.on('render', function (e) {
        clearTimeout(wto);
        
        // Notice the layoutstop event with the layout name.
        wto = setTimeout(function () {
            if (firstFullRendering) {
                console.log('First full rendering');
                cy.fit();
                cy.center();
                firstFullRendering = false;
            }
        }, 1200);

    });

    function showParentAccordion(element, display) {
        var parent = element.parent();
        
        var regex = RegExp('^accordion[0-9]+','ig');

        while (!regex.test(parent.attr('id'))) {
            
            parent.css('display', display);
            parent.css('overflow-y', "hidden");
            parent = parent.parent();
        }

        if (regex.test(parent.attr('id'))) {
            parent.css('display', display);
            parent.css('overflow-y', "hidden");
        }
        if (display === 'none') {
            $('.panel-group').css("margin-bottom", "0px");
        } 

    }

    $('#searchFilter').bind("propertychange change click keyup input paste", function (event) {

        var valueFilter = event.currentTarget.value;

        if(valueFilter !== ''){
            $('#filtersType').find('.collapse').collapse('show');
            
        }
        else{
            $('#filtersType').find('.collapse').collapse('hide');

            Object.keys(filtersTypes).forEach(function(filter) {
                showParentAccordion($('a[id="' + filter + '"]'), 'block');
            });
            $('.panel-group').css("margin-bottom", "20px");
        }

        var re = new RegExp(valueFilter, "i");

        Object.keys(filtersTypes).forEach(function(filter) {
            $('a[id="' + filter + '"]').css('display', 'block');
        });
        var matched = [];
        if (valueFilter != "") {
            Object.keys(filtersTypes).forEach(function(filter) {
                if (!re.test(filter)) {
                    $('a[id="' + filter + '"]').css('display', 'none');
                    showParentAccordion($('a[id="' + filter + '"]'), 'none');
                } else {
                    matched.push($('a[id="' + filter + '"]'));
                }
            });
            matched.forEach(function(filter) {
                showParentAccordion(filter, 'block');
            });
        }

    });

    var filterBodyRaw = {
        "name": "help",

        "command":
            {
                "type": "Help"
            }
    };

    $.ajax({
        data: JSON.stringify(filterBodyRaw),
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La récupération des commandes n\'a pas pu être effectuée.');
        },
        success: function (data) {
            Object.keys(data).forEach(function(el) {
                if (el != "Version" && el != "Hash" && el != "Filters" && el != "GroupFilters") {
                    commands.push(el);

                    $('#SaSCommand').append('<li><a href="javascript:void(0)" onclick="sendCommand(\'' +
                        el +
                        '\')">' +
                        el +
                        '</a></li>');
                }
            });

            commands.push("Resend");
            $('#SaSCommand').append('<li><a href="javascript:void(0)" onclick="resendCommand()">Resend</a></li>');
        }

    });

    $('.nav-pills li').on('shown.bs.tab', function () {
        if ($(this).find('a').attr('href') == '#data') {
            $('#panelAction').css('display', 'none');
            $('#buttonDetail').css('display', 'none');
            $('#buttonRegenerate').css('display', 'none');
            $('#detailsDiv').css('display', 'none');
        }
        else {
            $('#searchFilter').focus();
            $('#panelAction').css('display', 'block');
            $('#buttonDetail').css('display', 'block');
            $('#buttonRegenerate').css('display', 'block');
            $('#detailsDiv').css('display', 'block');
        }
    });

    var isSaved = false;
    $('#paramBtn').on('click',
        function(event) {
            var currentNodes = Object.assign({}, nodes);

            if (!isSaved && oldNodes != JSON.stringify(currentNodes)) {
                var href = $(this).attr('href');
                event.preventDefault();

                $('#confirmQuit').modal('show');

                $('#addConfirmedSave').on('click',
                    function() {
                        isSaved = true;
                        $('#confirmQuit').modal('hide');
                        saveModel();
                        $('#paramBtn')[0].click();
                    })

                $('#addNonConfirmedSave').on('click',
                    function() {
                        $('#confirmQuit').modal('hide');
                        isSaved = true;
                        $('#paramBtn')[0].click();
                    })
            } else {
                return true;
            }
        });
    $('#backBtn').on('click',
        function(event) {
            var currentNodes = Object.assign({}, nodes);

            if (!isSaved && oldNodes != JSON.stringify(currentNodes)) {
                var href = $(this).attr('href');
                event.preventDefault();

                $('#confirmQuit').modal('show');

                $('#addConfirmedSave').on('click',
                    function() {
                        isSaved = true;
                        $('#confirmQuit').modal('hide');
                        saveModel();
                        $('#backBtn')[0].click();
                    })

                $('#addNonConfirmedSave').on('click',
                    function() {
                        $('#confirmQuit').modal('hide');
                        isSaved = true;
                        $('#backBtn')[0].click();
                    })
            } else {
                return true;
            }
        });


    document.addEventListener('keydown', saveModelEvent, false);

});


function drawConnections(selector, isHtml, isInit, values) {
    
    if (isHtml) {
        values = $('#parent').val();
    }
  

    var nodeTarget = cy.filter('node[id = "' + nodes[selector].data.id + '"]');
    var existedEdges = cy.edges("[target='" + nodeTarget[0].id() + "']");
    var edgeToDelete = [];

    if (isInit != undefined && isInit === true) {
        var http = new XMLHttpRequest();
        var url = "/point/" + model.id + "/" + nodes[selector].data.name;
        http.open("GET", url, true);

        http.setRequestHeader("Content-type", "application/json");

        http.onload = function() {
            var point = JSON.parse(this.response);

            if (point.length > 0) {
                point = point[0];

                position_x = point.position_x;
                position_y = point.position_y;

                nodeTarget.renderedPosition("x", position_x);
                nodeTarget.renderedPosition("y", position_y);
                //var node = cy.filter('node[id = "' + nodes[selector].data.id + '"]');
            }
        };

        http.send();
    }
    ApplyImageToNode(nodes[selector].data.filter, nodeTarget);

    
    edge.forEach(function (singleEdge, key) {
        if (singleEdge.data.target == nodeTarget[0].id()) {
            edgeToDelete.push(key);
        }
    });

    edgeToDelete = edgeToDelete.reverse();
    edgeToDelete.forEach(function (key) {
        edge.splice(key, 1);
    });

    cy.remove(existedEdges);

    if (values && values != null) {
        values.forEach(function (value) {
            var node = cy.filter('node[id = "' + nodes[value].data.id + '"]');
            var nodeTarget = cy.filter('node[id = "' + nodes[selector].data.id + '"]');
            var exist = false;

            var newEdge = {data: {source: node[0].id(), target: nodeTarget[0].id()}};

            edge.push(newEdge);
            cy.add(newEdge);
        });
    }

}

function sendDataToServer() {
    var filterBodyRaw = generateJson(true, true);

    var waiting = $.notify({
        // options
        message: 'Le processus est en cours sur le serveur : ' + sessionStorage.getItem('srvUrl')
    }, {
        // settings
        type: 'info',
        delay: 0,
        animate: {
            enter: 'animated fadeInDown',
            exit: 'animated fadeOutUp'
        },
    });

    $.ajax({
        data: filterBodyRaw,
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            waiting.close();
            notifyError('Le serveur n\'as pas effectué le processus. Il se peut que le serveur ne fonctionne plus ou qu\'il n\'est pas possible de communiquer avec lui')
        },
        success: function (data) {
            waiting.close();
            if (data.Status && data.Status.Binding && data.Status.Binding == "OK") {
                $('#success_server').parent().parent().css('display', 'block');
                $('#success_server').val(JSON.stringify(data.Status));
                notifySuccess('Le serveur a effectué le processus : ' + JSON.stringify(data.Status));
                window.initBaseLogService();
                
                window.initMonitor();
               
                if (data.dataResponse) {
                    if (data.dataResponse.DataResult.type_0 == "IMGB64") {

                        var header = "";
                        if (data.dataResponse.DataResult.format_0 == "JPG") {
                            header = "data:image/jpg;base64,"
                        }
                        else if (data.dataResponse.DataResult.format_0 == "PNG") {
                            header = "data:image/png;base64,"
                        }

                        $('#imgDisplayModal').remove();

                        $('#resultData #resultDataBody').append('' +
                            '<div id="imgDisplayModal"><a href="' + header + data.dataResponse.DataResult.data_0 + '" download="result.' + data.dataResponse.DataResult.format_0 + '">' +
                            '<img id="image_result" style="width: 100%"/>' +
                            '</a></div>');

                        $('#image_result').attr('src', header + data.dataResponse.DataResult.data_0);

                        $('#resultData').find('.modal-body').css({
                            width: 'auto', //probably not needed
                            height: 'auto', //probably not needed
                            'max-height': '100%'
                        });

                        $('#resultData').modal('toggle');
                    }
                }
            }
            else {
                $('#error_server').parent().parent().css('display', 'block');
                $('#error_server').val(data.Status);
                notifyError('Le serveur n\'as pas effectué le processus. Message d\'erreur : ' + data.Status);
            }
        }

    });
   
}

function showDetail() {
    $('#details').val(generateJson(true));
    $('.showDetails').each(function () {
        $(this).show();
    });

}

function regenerateGraph() {
    resetGraph();

    setShema(JSON.parse($('#details').val()), true);
}

function displayOptions(selector) {
    var optBalise = this.cleanOptionBoard();


    var collection = cy.collection();


        optBalise.append("<select id='parent' multiple class='form-control'></select>");
        Object.keys(nodes).forEach(function (el) {
            if (nodes[selector] != undefined && nodes[el] != undefined) {
                if (nodes[selector].data.id != nodes[el].data.id) {
                    var test = selectParent(selector, el);
                    $('#parent').append('<option value="' + el + '" ' + test + '>' + nodes[el].data.name + '</option>');
                }
            }
        });
    //}


    var keyFilter = selector;
    var typeFilter = filtersTypes[nodes[keyFilter].data.filter];

    var el = '<div class="row">'
        + '<div class="col-lg-12">'
        + '<label for="attr_name">Name</label>'
        + '<input type="text" class="form-control" id="attr_name" placeholder="Name" value="' + nodes[keyFilter].data.name + '"/>'
        + '</div>'
        + '</div>';

    optBalise.append(el);

    if (typeFilter != undefined) {
        Object.keys(typeFilter["fields"]).forEach(function (key) {
            var el = '<div class="row">'
                + '<div class="col-lg-12">'
                + '<label for="attr_' + key + '">' + key + '</label>'
                + '<input type="text" class="form-control" id="attr_' + key + '" placeholder="' + key + '" value="' + nodes[keyFilter].data[key] + '"/>'
                + '</div>'
                + '</div>';

            optBalise.append(el);
        })
    }

    Object.keys(nodes[keyFilter].data).forEach(function (key) {
        var isExtra = -1;

        if (typeFilter != undefined) {
            isExtra = Object.keys(filtersTypes[nodes[keyFilter].data.filter]["fields"]).indexOf(key);
        }
        if (isExtra == -1 && key != 'filter' && key != 'name' && key != 'id' && key != 'need') {
            addExtraType(selector);
            $('#index_' + (countExtraType - 1)).val(key);
            $('#value_' + (countExtraType - 1)).val(nodes[keyFilter].data[key]);
        }
    });

    if (!isDataSource(nodes[selector].data.filter)) {
        optBalise.append('<a onclick="addExtraType(\'' + selector + '\', this)" class="btn btn-default btn-block" style="margin: 10px 0px;">Add New Option</a>');
    }

    optBalise.append("<a href='javascript:void(0)' class='btn btn-default' id='option_validation' class='" + selector + "'>Valider</a>");

    optBalise.append("<a href='javascript:void(0)' class='btn btn-danger' id='delete_node' style='float: right;' class='" + selector + "'>Supprimer</a>");

    $('#option_validation').on('click',
        function(ev) {
            drawConnections(selector, true, false);
            saveEdit(selector);
        });

    $('#delete_node').on('click',
        function(ev) {
            deleteModal('node', selector);
        });

    $('#optionspanel').css('display', "inline-block");
}

function saveEdit(selector) {
    Object.keys(nodes[selector].data).forEach(function (key) {
        var isExtra = Object.keys(filtersTypes[nodes[selector].data.filter]).indexOf(key);
        if (isExtra == -1 && key != 'filter' && key != 'name' && key != 'id' && key != 'need' && key != 'code_impl') {
            delete nodes[selector].data[key];
        }
    });

    $("#options :input").each(function () {
        var input = $(this)[0];
        var key = input.id;
       
        var extraRegexIndex = /^index_/;
        var extraRegexValue = /^value_/;
        var extraRegexCodeImpl = /code_impl/;
        var isIndexExtra = extraRegexIndex.test(key);
        var isValueExtra = extraRegexValue.test(key);
        var isCodeImpl = extraRegexCodeImpl.test(key);

        if (key !== "parent" && !isIndexExtra && !isValueExtra && !isCodeImpl) {
            key = key.substring(key.indexOf("attr_") + 5);

            if (key === "name") {
                var node = cy.nodes("[id='" + nodes[selector].data.id + "']");
                node.data().name = input.value;
            }
            else {
                nodes[selector].data[key] = input.value;
            }
        }
        else if (isIndexExtra) {
            var id = key.substring(6);
            nodes[selector].data[input.value] = $("#value_" + id)[0].value;
        }

    });

    var split = selector.split('_');
    var idInList = split[split.length - 1];

    var listOfConfigData = $("input[id^='config_" + idInList + "']");

    if (listOfConfigData.length > 0) {
        listOfConfigData.toArray().forEach(function (input) {
            var split = $(input).attr('id').split('_');
            var index = split[split.length - 1];

            var element = $('#attr_' + idInList + '_' + index);

            if (element.val() == 'name') {
                var oldName = $(input).val();

                changeNameNeed(oldName, $('#attr_' + element.val()).val())
            }

            $(input).val($('#attr_' + element.val()).val());
        });
    }

    notifySuccess('L\'élément a bien été enregistré');
}


function deleteItem(element, upper) {
    if (upper) {
        $(element).parent().parent().remove();
    }
    else {
        $(element).parent().remove();
    }
}

function deleteItemDroped() {
    $('#itemDroped' + $('#idItem').val()).remove();
    delete tabData[$('#idItem').val()];
}

function addConfiguration(idNode) {

    var htmlConfiguration = '<div id="config_container_all_' + idNode + '">' +
        '<button id="button_collapse_' + idNode + '" class="btn btn-default button-collapse-data" type="button" data-toggle="collapse" ' +
        'data-target="#config_container_' + idNode + '" aria-expanded="false" aria-controls="collapseExample">' +
        '    ' +
        '  </button>' +
        '<div class="collapse collapse-data" aria-expanded="false"  id="config_container_' + idNode + '">';

    htmlConfiguration = htmlConfiguration + '<div class="removeData-container"><div class="removeData">'
        + '<a href="javascript:void(0)" id="delete_config_' + idNode + '" class="btn btn-danger btn-xs" data-id="' + idNode + '"><span class="glyphicon glyphicon-remove"></span></a>'
        + '</div></div>'
        + '</div></div><p></p>';

    $('#configuration').append(htmlConfiguration);

    $('a[id^="delete_config_' + idNode + '"]').on('click', function (ev) {
        var id = $(this).attr('data-id');

        deleteModal(null, id).then(function (id) {
            deleteNode($("#delete_config_" + id).attr('selector'));
            deleteItem($("#delete_config_" + id).parent().parent(), true);
            countDataInfo--;

        })

    });

    countDataInfo++;

    if (countDataInfo > 0) {
        $('#configuration').css('display', 'block');
    }
}

function showDetailData() {
    $('#datasModal').modal('show');
}

var countExtraType = 0;

function addExtraType(selector, el) {

    var htmlConfiguration = '<div class="row" style="margin-top: 10px;">'
        + '<div class="col-lg-4">'
        + '<input type="text" class="form-control" id="index_' + countExtraType + '" placeholder="Attribute" required/>'
        + '</div>'
        + '<div class="col-lg-6">'
        + '<input type="text" class="form-control"  id="value_' + countExtraType + '" placeholder="Value" />'
        + '</div>'
        + '<div class="col-lg-2">'
        + '<a href="javascript:void(0)" id="delete_extra_' + countExtraType + '" class="btn btn-danger btn-xs" data-id="' + countExtraType + '"><span class="glyphicon glyphicon-remove"></span></a>'
        + '</div>'
        + '</div><p></p>';

    if (el != null) {
        $(htmlConfiguration).insertBefore(el);
    }
    else {
        $('#options').append(htmlConfiguration);
    }

    $('a[id^="delete_extra_' + countExtraType + '"]').on('click', function (ev) {
        var id = $(this).attr('data-id');

        deleteModal(null, selector).then(function () {
            if (nodes[selector].data[$('#index_' + id).val()] != undefined) {
                delete nodes[selector].data[$('#index_' + id).val()];
            }

            deleteItem("#delete_extra_" + id, true);

            countExtraType--;
        })

    });

    countExtraType++;
}

function deleteNode(selector) {
    if (nodes[selector] != undefined) {
        this.deleteEdge(selector);

        var nodeTarget = cy.filter('node[id = "' + nodes[selector].data.id + '"]');

        Object.keys(nodes).forEach(function (key) {
            if (nodes[key].data.id == nodeTarget[0].data().id) {
                var node = cy.nodes("[id='" + nodes[key].data.id + "']");
                cy.remove(node);
                delete nodes[key];
            }

        });
    }

    notifySuccess('L\'élément a bien été supprimé');
}

function deleteEdge(selector) {
    var nodeTarget = cy.nodes("[id='" + nodes[selector].data.id + "']");

    if (nodeTarget.length > 0) {
        var existedEdges = nodeTarget[0].connectedEdges();
        edge.forEach(function (singleEdge, key) {
            if (singleEdge.data.source == nodeTarget[0].id() || singleEdge.data.target == nodeTarget[0].id()) {
                edge.splice(key, 1);
            }
        });

        cy.remove(existedEdges);
    }
}

function cleanOptionBoard() {
    var optBalise = $('#options');
    optBalise.empty();

    return optBalise;
}

function resetGraph() {
    nodes = [];
    edge = [];

    cy.elements().remove();

    var dataSelector = $('#configuration').children();

    dataSelector.each(function () {
        $(this).remove();
        countDataInfo--;
    })
}

function getFilterByName(name) {
    var keyFound = null;

    Object.keys(nodes).forEach(function(key) {
        if (nodes[key].data.name == name) {
            keyFound = key;
        }
    });

    return keyFound;
}

function selectParent(selector, parent) {
    var isSelected = false;

    edge.forEach(function (simpleEdge) {
        if (simpleEdge.data.target == nodes[selector].data.id && simpleEdge.data.source == nodes[parent].data.id) {
            isSelected = true;
        }
    });

    if (isSelected) {
        return 'selected';
    }
    else {
        return '';
    }

}

function saveModel(el, needReload) {
    if (needReload == undefined) {
        needReload = true;
    }

    var btnContent = $(el).text();
    $(el).html('<div class="loader"></div>');

    var json = generateJson(false, true);

    var data = {};
    data['version'] = sessionStorage.getItem('version');
    data['name'] = $('#graphicName').val();
    data['json'] = json;
    data['id_of_next_element'] = countElement;
    data['points'] = [];

    cy.nodes().toArray().forEach(function(node){
        var point = {};
        point.selector = node.data().name;
        point.position_x = node.position().x;
        point.position_y = node.position().y;

        data['points'].push(point);
    });

    var http = new XMLHttpRequest();

    if (model != null && model != "") {
        var url = "/model/" + model.id;
        http.open("PUT", url, true);
    }
    else {
        var url = "/model";
        http.open("POST", url, true);
    }

    http.setRequestHeader("Content-type", "application/json");

    http.onload = function () {
        notifySuccess('L\'élément a bien été enregistré');
        $(el).html(btnContent);

        if (needReload) {
            var id = JSON.parse(this.response);
            window.location.pathname = '/model/' + id['id'];
        }
    };

    http.send(JSON.stringify(data));
}

function sendCommand(command) {
    var filterBodyRaw = {
        "name": command,

        "command":
            {
                "type": command
            }
    };

    $.ajax({
        data: JSON.stringify(filterBodyRaw),
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La commande n\'a pas pu être effectuée.')
        },
        success: function (data) {
            notifySuccess('La commande ' + command + ' a été effectée.')
        }

    });
}

function resendCommand() {
    var filterBodyRaw = {
        "name": 'Kill',

        "command":
            {
                "type": 'Kill'
            }
    };

    $.ajax({
        data: JSON.stringify(filterBodyRaw),
        url: sessionStorage.getItem('srvUrl') + '/json',
        type: 'POST',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La commande n\'a pas pu être effectuée.');
        },
        success: function (data) {
            notifySuccess('La commande kill a été effectée.');
            sendDataToServer();
        }

    });
}


//function onChange(event) {
//    var reader = new FileReader();
//    reader.onload = onReaderLoad;
//    reader.readAsText(event.target.files[0]);
//}

//function onReaderLoad(event){
//    var obj = JSON.parse(event.target.result);
//    setShema(obj, false);
//}


function saveNewModel() {
    var json = {
        "name": $('#newGraphicName').val(),

        'filters': [],
        'orchestrator': 'DefaultScheduler'
    };

    json = JSON.stringify(json, null, "\t");
    //generateJson(false);}

    var data = {};
    data['version'] = sessionStorage.getItem('version');
    data['name'] = $('#newGraphicName').val();
    data['json'] = json;

    var http = new XMLHttpRequest();

    var url = "/model";
    http.open("POST", url, true);

    http.setRequestHeader("Content-type", "application/json");

    http.onload = function () {
        notifySuccess('L\'élément a bien été enregistré');

        var id = JSON.parse(this.response);
        window.location.pathname = '/model/' + id['id'];
    };

    http.send(JSON.stringify(data));
}

var addModelModal = function(){
    $('#addModelModal').modal('toggle');

    //document.getElementById('importFile').addEventListener('change', onChange);

    $('#confirmedAddBtn').on('click',
        function(ev) {
            if (ev.target.checkValidity()) {
                ev.preventDefault();
                saveNewModel();
            }
        });
}

function saveCode() {
    var target = $('#Editor').data("target");
    var cm = $('#codeArea').parent().find('.CodeMirror')[0].CodeMirror;


    target.code_impl = cm.getValue();
    displayOptions(target.filter + '_' + target.id);
    notifySuccess('Python code was successfully saved');
    //saveEdit(target.filter + '_' + target.id);
}

function closeEditor() {
    document.removeEventListener('keydown', keyEventEditor);
    document.addEventListener('keydown', saveModelEvent);
    $('#Editor').css('visibility', 'hidden');
    $("#bodyBoard").focus();
}