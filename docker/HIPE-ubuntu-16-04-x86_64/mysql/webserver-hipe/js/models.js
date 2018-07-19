$(function () {
    $('a[id^="deleteModels"]').on('click', function (ev) {
        var id = $(this).attr('model-id');

        deleteModal('models', id).then(function(id){
            deleteElement(id);
        });
    });

    $('#addModalBtn').on('click', function(){
        addModelModal();
    })

    var filterBodyRaw = {
        "name" : "filters",

        "command" :
            {
                "type" : "filters"
            }
    };

    $.ajax({
        url: '/filtersTypes',
        type: 'GET',
        dataType: 'JSON',
        error: function (xhr, status, error) {
            notifyError('La récupération des types de filtres n\'a pas pu être effectuée.')
        },
        success: function (data) {
            filtersTypes = data;
        }
    });
});

var addModelModal = function(){
    $('#addModelModal').modal('toggle');

    document.getElementById('importFile').addEventListener('change', onChange);

    $('#confirmedAddBtn').on('click', function(ev){
        if (ev.target.checkValidity()) {
            ev.preventDefault();
            saveModel()
        }
    })
}

var cleanTableLine = function(selector){
    $('#model_' + selector).remove();
}

var deleteElement = function(model_id){
    var http = new XMLHttpRequest();

    var url = "/model/" + model_id;
    http.open("DELETE", url, true);

    http.setRequestHeader("Content-type", "application/json");

    http.send();
}


function saveModel(){

    var json = generateJson(false);

    var data = {};
    data['version'] = sessionStorage.getItem('version');
    data['name'] = $('#graphicName').val();
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

function onChange(event) {
    var reader = new FileReader();
    reader.onload = onReaderLoad;
    reader.readAsText(event.target.files[0]);
}

function onReaderLoad(event){
    var obj = JSON.parse(event.target.result);
    setShema(obj, false);
}