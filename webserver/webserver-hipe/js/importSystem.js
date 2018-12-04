$(function () {
    $('#addConfirmedBtn').on('click', function(){

        var listOfConfigsDatas = $("div[id^='config_container_all_']");

        if(listOfConfigsDatas.length > 0) {
            listOfConfigsDatas.toArray().forEach(function (el) {
                var split = $(el).attr('id').split('_');
                var idInList = split[split.length - 1];

                var idNodeFilter = $('#config_container_' + idInList).attr('type') + '_' + idInList;

                var listOfConfigData = $("input[id^='config_" + idInList + "']");

                if(listOfConfigData.length > 0) {
                    listOfConfigData.toArray().forEach(function (input) {
                        var split = $(input).attr('id').split('_');
                        var index = split[split.length - 1];

                        var element = $('#attr_' + idInList + '_' + index);

                        if(element.val() == 'name'){
                            var oldName = nodes[idNodeFilter].data.name;

                            changeNameNeed(oldName, $(input).val())
                        }

                        nodes[idNodeFilter].data[element.val()] = $(input).val();
                    });
                }
            });
            notifySuccess('L\'élément a bien été enregistré');
        }
    })
});

function setShema(json, graphical) {
    var rootName = "";
    var indexDataSource = 0;

    try {
        Object.keys(json).forEach(function (key) {
            if (graphical != null && graphical == true) {
                if (key == 'data') {
                    Object.keys(json[key]['datasource']).forEach(function (keyData) {
                        var type = Object.keys(json[key]['datasource'][keyData])[0];

                        var currentId = countDataInfo - 1;


                        if (type == "IMGB64") {

                            var data = Object.assign({}, json[key]['datasource'][keyData]);
                            imgsB64.push(data.data);
                            json[key].data = json[key].data.substring(0, 160) + '...';
                        }

                        else if (type == "LISTIO") {
                            json[key].array.forEach(function (filter, index) {
                                Object.keys(json[key]['array'][index]).forEach(function (filterParam) {
                                    if (json[key]['array'][index][filterParam] == "IMGB64") {
                                        imgsB64.push(json[key]['array'][index].data);
                                        json[key]['array'][index].data = json[key]['array'][index].data.substring(0, 160) + '...';
                                    }
                                })
                            })
                        }

                        var selector = addElementInGraph(type, null, json[key]['datasource'][keyData][type], graphical);
                        drawConnections(selector, false, true, []);

                        $('#attr_' + (idNode)).val(type);

                        var split = selector.split("_");
                        var idNode = split[split.length - 1];

                        addConfiguration(idNode);
                        $("#delete_config_" + (idNode)).attr('selector', selector);
                        $('#button_collapse_' + (idNode)).html('<span class="fa fa-chevron-down"></span> ' + type + ' (' + nodes[selector].data.name + ')');

                        if(indexDataSource == 0){
                            $('#button_collapse_' + (idNode)).click();
                        }

                        indexDataSource++;

                        addDataConfiguration(type, json[key]['datasource'][keyData][type], idNode);
                        delete json[key]['datasource'][keyData];
                    });

                }
            }
        });
        Object.keys(json).forEach(function (key) {

            if (key == 'filters') {
                json[key].forEach(function (filter) {
                    var selector = addElementInGraph(Object.keys(filter)[0], null, filter[Object.keys(filter)[0]], graphical);

                    if (graphical != null && graphical == true) {
                        var formattedParent = [];
                        if (filter[Object.keys(filter)]['need'] != undefined) {
                            var isFind = false;

                            filter[Object.keys(filter)]['need'].forEach(function (parent, index) {

                                var nodeTarget = cy.filter(function (element, i) {
                                    if (element.isNode() && element.data("name") == parent) {
                                        return true;
                                    }
                                    return false;
                                });

                                if (nodeTarget.length > 0 && !isFind) {
                                    Object.keys(nodes).forEach(function (key) {
                                        if (nodes[key].data.id == nodeTarget[0].data().id) {
                                            formattedParent.push(key);
                                            isFind = false;
                                        }
                                    });
                                }

                            });
                        }
                        drawConnections(selector, false, true, formattedParent);
                    }
                });
            }
            if (graphical != null && graphical == true) {

                if (key == 'name') {
                    $('#graphicName').val(json[key]);
                }
                if (key == 'orchestrator') {
                    $('#orchestrator').val(json[key]);
                }
            }
        });

        Object.keys(nodes).forEach(function(keyNode){
            if(isDataSource(nodes[keyNode].data.filter)){
                var formattedParent = [];
                if (nodes[keyNode].data.need != undefined) {
                    var isFind = false;

                    nodes[keyNode].data.need.forEach(function (parent, index) {

                        var nodeTarget = cy.filter(function (element, i) {
                            if (element.isNode() && element.data("name") == parent) {
                                return true;
                            }
                            return false;
                        });

                        if (nodeTarget.length > 0 && !isFind) {
                            Object.keys(nodes).forEach(function (key) {
                                if (nodes[key].data.id == nodeTarget[0].data().id) {
                                    formattedParent.push(key);
                                    isFind = false;
                                }
                            });
                        }

                    });
                }
                drawConnections(keyNode, false, true, formattedParent);
            }
        });

        if (graphical != null && graphical == true) {
            orderNodesInGraph();
        }
        var currentNodes = Object.assign({}, nodes);
        oldNodes = JSON.stringify(currentNodes);

        notifySuccess('L\'élément a bien été importé');

    }
    catch (err) {
        console.log(err);
        notifyError('Une erreur est survenu lors de l\'importation du fichier. Verifiez que le JSON est valide : ' + err.message);
        $('#details').val(JSON.stringify(json, null, 2));
    }


}

var existIn = function(arr1, arr2) {
    var ret = [];
    for(var i in arr1) {
        if(arr2.indexOf( arr1[i] ) > -1){
            ret.push( arr1[i] );
        }
    }
    return ret;
};

function orderNodesInGraph() {
    var nodesSorted = sortNodes();
    var y = 50;
    var containerWidth = $('.boardcard').width();
    var maxColumn = 0;
    var columns = [];

    nodesSorted.forEach(function (nodesSorted, key) {
        var numOfLine = Object.keys(nodesSorted).length;
        x = containerWidth / (numOfLine + 1);

        Object.keys(nodesSorted).forEach(function (keyInSorted) {
            var nodeTarget = cy.nodes("[id='" + keyInSorted + "']");
            nodeTarget[0].position({
                x: x,
                y: y
            })
            x = x + (containerWidth / (numOfLine + 1));

            if(nodeTarget.data.need != undefined) {
                if (maxColumn < nodeTarget.data.need.length) {
                    maxColumn = nodeTarget.data.need.length;
                }
            }
        });

        y = y + 150;
    });

    /*var lefter = 150 - (150 * (maxColumn / 2));

    columns[0] = lefter;
    for(var ind = 1; ind < maxColumn; ind++){
        columns[ind] = lefter + (150 * ind);
    }
*/
    var columnIndex = 0;

    nodesSorted.reverse().forEach(function (nodesSortedReverse, key) {
        var numOfLine = Object.keys(nodesSorted[key]).length;
        x = containerWidth / (numOfLine + 1);

        Object.keys(nodesSortedReverse).forEach(function (keyInSorted) {
            var nodeTarget = cy.nodes("[id='" + keyInSorted + "']");
            if (nodeTarget.data().need != undefined && key != 0) {
                if (nodeTarget.data().need.length > 0) {
                    var nbParent = nodeTarget.data().need.length;
                    var newX = nodeTarget[0].position().x;

                    if(nbParent != 1) {


                        nodeTarget.data().need.forEach(function (parentNode) {
                            var nodeParentTarget = cy.nodes("[name='" + parentNode + "']");

                            var positionSorted = 0;
                            var elementInArray = 0;
                            var nbElementInLine = 0;

                            nodesSorted.forEach(function (nodesInForEach, sortedKey) {
                                Object.keys(nodesInForEach).forEach(function (nodeKey, test) {
                                    if (nodeKey == nodeParentTarget.data().id) {
                                        positionSorted = sortedKey;
                                    }
                                    if (nodeKey == nodeTarget.data().id) {
                                        elementInArray = test + 1;
                                    }
                                })
                            })
                            if (key == (positionSorted - 1)) {
                                newX = nodeParentTarget[0].position().x;
                                if (elementInArray > 1) {
                                    var feet = newX * (1 / Object.keys(nodesSorted[positionSorted - 1]).length);
                                    newX = feet + (feet * elementInArray);

                                    if (Object.keys(nodesSorted[positionSorted - 1]).length > 2) {
                                        newX = nodeParentTarget[0].position().x;
                                    }
                                }
                                else {
                                    newX = newX;
                                }

                            }
                            else {
                                newX = newX / nbParent;
                            }

                            nodeTarget[0].position({
                                x: newX
                            })

                            elementInArray++;


                        })
                    }
                }

            }
            else {

                var nodeTarget = cy.nodes("[id='" + keyInSorted + "']");
                var newX = nodeTarget[0].position().x;

                nodeTarget[0].position({
                    x: x
                })

                x = x + (containerWidth / (numOfLine + 1));
            }
        })
    })
}

function addDataConfiguration(type, containt, idNode){
    var currentId = countDataInfo - 1;
    var typeFilter = filtersTypes[type];

    var optBalise = $('#config_container_' + idNode + '');

    optBalise.addClass('data-type');
    optBalise.attr('type', type);

    var numberElement = 0;

    Object.keys(typeFilter["fields"]).forEach(function (attr) {
        var value = "";
        if(containt){
            if(containt[attr] != undefined && containt[attr] != "undefined") {
                value = containt[attr];
            }
        }

        var el = '<div class="row ">'
            + '<div class="col-lg-3">'
            + '<input type="text" class="form-control" id="attr_' + idNode + '_' + numberElement + '" placeholder="' + attr + '" value="' + attr + '" disabled="disabled"/>'
            + '</div>'
            + '<div class="col-lg-9">'
            + '<input type="text" class="form-control" id="config_' + idNode + '_' + numberElement + '" placeholder="' + attr + '" value="' + value + '"/>'
            + '</div>'
            + '</div>';

        optBalise.append(el);

        numberElement++;
    });

    var name = "";
    if(containt) {
        if ($.isArray(containt) || $.isPlainObject(containt)) {
            name = containt.name;
        }
        else {
            name = containt;
        }
    }

    var el = '<div class="row ">'
        + '<div class="col-lg-3">'
        + '<input type="text" class="form-control" id="attr_' + idNode + '_' + numberElement + '" placeholder="name" value="name" disabled="disabled"/>'
        + '</div>'
        + '<div class="col-lg-9">'
        + '<input type="text" class="form-control" id="config_' + idNode + '_' + numberElement + '" placeholder="name" value="' + name + '"/>'
        + '</div>'
        + '</div>';

    optBalise.append(el);

    //$('#config_' + (currentId)).val(JSON.stringify(json[key]['datasource'][keyData][type]));
}

function changeNameNeed(oldName, name){
    Object.keys(nodes).forEach(function (nodeKey) {
        if(nodes[nodeKey].data['need'] != undefined){
            Object.keys(nodes[nodeKey].data.need).forEach(function (key) {
                if(nodes[nodeKey].data.need[key] == oldName){
                    nodes[nodeKey].data.need[key] = name;
                }
            })
        }
    })
}