var datas = {};
var isListio = false;

var countConfiguration = 0;
var cropLine = 0;

var generateDom = function () {
    return new Promise(
        function (resolve) {
            var dataBody = $('#dataBody');
            var newLine = "<div class='row margin-line' id='line_" + countConfiguration + "'></div>";

            if (isListio) {
                $(newLine).insertBefore($('#addData'));
            }
            else {
                dataBody.append(newLine);
            }

            $('#line_' + countConfiguration).append("<div class='col-lg-3' id='firstColumn_" + countConfiguration + "'></div>");
            $('#firstColumn_' + countConfiguration).append("<select id='typeData_" + countConfiguration + "' class='form-control'></select>");

            var isSelected = false;
            Object.keys(datas).forEach(function (data) {
                if (!isListio) {
                    var selected = '';
                    if (data != 'LISTIO' && !isSelected) {
                        selected = 'selected';
                        isSelected = true;
                    }
                    $('#typeData_' + countConfiguration).append('<option value="' + data + '" ' + selected + '>' + data + '</option>');
                }
                else if (data != 'LISTIO') {
                    $('#typeData_' + countConfiguration).append('<option value="' + data + '">' + data + '</option>');
                }
            });

            $('#line_' + countConfiguration).append("<div class='col-lg-9' id='secondColumn_" + countConfiguration + "'>" +
                "<div class='row'><div class='col-lg-10 inputZone'></div><div class='col-lg-2 deleteZone' style='padding: 10px;'></div></div></div>");
            $('#secondColumn_' + countConfiguration + ' .inputZone').append('<input type="text" placeholder="Path" id="pathInput" class="form-control"/>');

            $('#secondColumn_' + countConfiguration + ' .deleteZone').append('<a href="javascript:void(0)" class="btn btn-danger btn-xs btn-flex"' +
                ' onclick="removeLine(' + countConfiguration + ')">' +
                '<span class="glyphicon glyphicon-remove"></span>' +
                '</a>');
            eventListenerChange('#typeData_' + countConfiguration);

            countConfiguration++;

            resolve(countConfiguration - 1);
        }
    );
};

var importData = function () {
    if (countDataInfo > 0) {

        var type = $('#attr_0').val();


        if (type == 'LISTIO') {
            generateDom().then(function () {
                importListio(type);
            });
        }
        else if(type == 'IMGB64'){
            generateDom().then(function () {
                importBase64();
            });
        }

        else if(type == 'SQR_CROP'){
            generateDom().then(function () {
                importSqrCrop();
            });
        }

        else {
            generateDom().then(function () {
                importDefault(type);
            });
        }
    }
    else {
        generateDom();
    }
}

var specificDisplay = function (el) {
    var selectValue = el.val();

    if (selectValue != undefined) {
        var id = el.attr('id');
        var selectedLine = id.split("typeData_")[1];

        $('#line_' + selectedLine).find('input').css('display', 'block');

        if ($('#line_' + selectedLine).attr('typeFilter') == 'SQR_CROP') {
            $('#line_' + selectedLine + ' div[id^="full_crop_line_"]').each(function(){
                removeCropLine($(this));
            })

            $('#addCrop').remove();
        }
        else if ($('#line_' + selectedLine).attr('typeFilter') == 'IMGB64') {
            $('#line_' + selectedLine + ' #fileInfo').replaceWith('<input type="text" placeholder="Path" id="pathInput" class="form-control"/>');
            $('#base64_line_' + selectedLine).remove();
        }

        if (isListio && id == "typeData_0") {
            if (selectValue != 'LISTIO') {
                removeListio(el, false)
            }
        }

        if (selectValue == 'LISTIO') {
            isListio = true;

            $('#line_' + selectedLine).find('input').css('display', 'none');
            $('#dataBody').append("<div class='row margin-line' id='addData'><div class='col-lg-12'>" +
                "<a href='javascript:(void)' onclick='generateDom()' class='btn btn-default'>Ajouter data</a></div></div>");
        }

        else if (selectValue == 'SQR_CROP') {
            $('#secondColumn_' + selectedLine).append("<div class='row margin-line' id='addCrop'><div class='col-lg-12'>" +
                "<a href='javascript:(void)' class='btn btn-default'>Ajouter crop</a></div></div>");

            addCropLine(selectedLine);

            $('#addCrop').on('click', function () {
                addCropLine(selectedLine);
            })
        }

        else if(selectValue == 'IMGB64'){
            addBase64Line(selectedLine);
        }

        $('#line_' + selectedLine).attr('typeFilter', selectValue);
    }
}

var confirmedData = function () {
    var dataSelector = $('#configuration').find('.row');

    dataSelector.each(function () {
        $(this).remove();
        countDataInfo--;
    });

    var shouldJsonify = false;
    var json = {};

    if (isListio) {
        json.array = [];
    }

    imgsB64 = [];

    $('select[id^="typeData_"]').each(function () {
        var idElement = $(this).attr('id');
        var id = idElement.split("typeData_")[1];

        if (isListio && id == 0) {
            shouldJsonify = true;
        }
        else if ($(this).val() == 'SQR_CROP') {

            var data = {};
            var lines = $('div[id^="crop_line_' + id + '"]');

            var crops = [];

            lines.each(function () {
                var inputs = $(this).find('input');
                inputs.each(function () {
                    crops.push($(this).val());
                })
            })

            data.type = 'SQR_CROP';
            data.crop = crops;
            data.IMGF = $('#line_' + id).find('#pathInput').val();

            if (!shouldJsonify) {
                json = data;

                addConfiguration();
                $('#attr_' + (countDataInfo - 1)).val($(this).val());
                $('#config_' + (countDataInfo - 1)).val(JSON.stringify(json));
            }
            else {
                json.array.push(data);
            }
        }
        else if ($(this).val() == 'IMGB64') {
            var data = {};

            data.type = 'IMGB64';
            data.format = $('#line_' + id + ' #format').val();
            if(data.format == "RAW"){
                data.channels = $('#line_' + id + ' #channel').val();
            }

            data.width = $('#line_' + id + ' #width').val();
            data.height = $('#line_' + id + ' #height').val();

            data.data = $('#line_' + id + ' #dataBase64_' + id).val();

            imgsB64.push(data.data);
            data.data = data.data.substring(0, 160) + '...';

            if (!shouldJsonify) {
                json = data;

                addConfiguration();
                $('#attr_' + (countDataInfo - 1)).val($(this).val());
                $('#config_' + (countDataInfo - 1)).val(JSON.stringify(json));
            }
            else {
                json.array.push(data);
            }
        }
        else {
            var input = $('#line_' + id).find('#pathInput');
            var value = "";
            if (input.length > 0) {
                value = input.val();
            }

            if (!shouldJsonify) {
                var val = {};
                val.path = value;

                addConfiguration();
                $('#attr_' + (countDataInfo - 1)).val($(this).val());
                $('#config_' + (countDataInfo - 1)).val(JSON.stringify(val));
            }
            else {
                var data = {};

                data.type = $(this).val();
                data.path = value;
                json.array.push(data);
            }
        }

    });

    if (isListio) {
        addConfiguration();
        $('#attr_' + (countDataInfo - 1)).val('LISTIO');
        $('#config_' + (countDataInfo - 1)).val(JSON.stringify(json));
    }

    notifySuccess('Les données ont été modifiées.');

}

var addCropLine = function (selectedLine) {

    var newLine = '<div class="row margin-line" id="full_crop_line_' + selectedLine + '_' + cropLine + '">' +
        '<div class="col-lg-10"><div class="row margin-line" id="crop_line_' + selectedLine + '_' + cropLine + '"></div></div>' +
        '<div class="col-lg-2 deleteZone" style="padding: 10px;"></div></div>';
    $(newLine).insertBefore($('#addCrop'));

    for (var i = 0; i < 4; i++) {
        $('#crop_line_' + selectedLine + '_' + cropLine).append('<div class="col-lg-3"><input type="text" class="form-control crop-line"></div>')
    }

    var selector = '#full_crop_line_' + selectedLine + '_' + cropLine;
    $(selector + ' .deleteZone').append('<a href="javascript:void(0)" class="btn btn-danger btn-xs btn-flex"' +
        ' onclick="removeCropLine(\'' + selector + '\')">' +
        '<span class="glyphicon glyphicon-remove"></span>' +
        '</a>');

    cropLine++;

    return 'crop_line_' + selectedLine + '_' + cropLine - 1;
}

var addBase64Line = function (selectedLine) {
    $('#line_' + selectedLine + ' #pathInput').replaceWith('<div id="fileInfo">' +
        '<input type="file" class="form-control btn btn-default" onchange="uploadData(this)" textarea-id="' + selectedLine + '"/>' +
        '<textarea id="dataBase64_' + selectedLine + '" class="form-control" style="display: none;"></textarea></div>')

    var newLine = '<div class="row margin-line" id="base64_line_' + selectedLine + '"></div>';
    $('#secondColumn_' + selectedLine).append(newLine);

    $('#base64_line_' + selectedLine).append(
        '<div class="col-lg-3">' +
        '<select id="format" class="form-control">' +
        '<option value="RAW">RAW</option>' +
        '<option value="JPG">JPG</option>' +
        '<option value="PNG">PNG</option>' +
        '</select>' +
        '</div>'
    );
    $('#base64_line_' + selectedLine).append(
        '<div class="col-lg-3">' +
        '<select id="channel" class="form-control">' +
        '<option value="1">1</option>' +
        '<option value="2">2</option>' +
        '<option value="3">3</option>' +
        '<option value="4">4</option>' +
        '</select>' +
        '</div>'
    );
    $('#base64_line_' + selectedLine).append(
        '<div class="col-lg-3"><input type="text" placeholder="Width" id="width" class="form-control"/></div>'
    );
    $('#base64_line_' + selectedLine).append(
        '<div class="col-lg-3"><input type="text" placeholder="Height" id="height" class="form-control"/></div>'
    );

}

var eventListenerChange = function (id) {
    $(id).change(function (e) {
        specificDisplay($(this));
    });
}

var removeListio = function(el, hard){
    isListio = false;

    if(hard != null && !hard) {
        $('select[id^="typeData_"]').each(function () {
            var id = $(this).attr('id');
            var selectedLine = id.split("typeData_")[1];

            if (selectedLine != "0") {
                $('#line_' + selectedLine).remove();
                countConfiguration--;
            }
        });

        if($(el).val() == "IMGB64"){
            $('#dataBody select[value="IMGB64"]').each(function(index){
                var id = $(this).attr('id');
                var numLine = id.split("typeData_")[1];

                if(numLine == selectedLine){
                    imgsB64.splice(index, 1);
                }
            })
        }

        var id = el.attr('id');
        var selectedLine = id.split("typeData_")[1];

        $('#line_' + selectedLine).find('input').css('display', 'block');
    }

    $('#addData').remove();
}

var removeLine = function (id) {

    var alreadyRemoved = false;
    if($("#typeData_" + id).val() == 'LISTIO'){
        removeListio($("#typeData_" + id), false);
    }

    $('#line_' + id).remove();

    if(countConfiguration <= 1){
        countConfiguration--;
        generateDom()
    }
    else{
        countConfiguration--;
    }
}

var removeCropLine = function (selector) {

    $(selector).remove();

    cropLine--;
}

var uploadData = function(el){
    var extension = $(el).val().match(/^(.*)\.([^.]*)$/i);
    extension = extension[2].toUpperCase();

    var reader = new FileReader();

    if(extension == "RAW"){
        reader.readAsBinaryString(el.files[0]);
    }
    else{
        reader.readAsDataURL(el.files[0]);
    }

    reader.onload = function () {
        var id = $(el).attr('textarea-id');

        $('#line_' + id + ' #channel').css('display', 'block');
        $('#dataBase64_' + id).val(reader.result.split(',')[1]);

        $('#line_' + id + ' #format').val(extension);

        var image = new Image();
        image.src = reader.result;
        image.onload = function() {
            $('#line_' + id + ' #width').val(image.width);
            $('#line_' + id + ' #height').val(image.height);
        };

        if(extension == "JPG" || extension == "PNG"){
            $('#line_' + id + ' #channel').css('display', 'none');
        }
    };
    reader.onerror = function (error) {
        console.log('Error: ', error);
    };

}

var importListio = function(type){
    $('#line_' + countConfiguration - 1).attr('typeFilter', type);

    var values = JSON.parse($('#config_0').val());
    $('#typeData_' + (countConfiguration - 1)).val('LISTIO');

    specificDisplay($('#typeData_' + (countConfiguration - 1)));

    values.array.forEach(function (value) {
        generateDom().then(function (index) {
            if(value.type == "IMGB64"){
                importBase64(value, index);
            }
            else if(value.type == "SQR_CROP"){
                importSqrCrop(value, index);
            }
            else{
                importDefault(value.type, value, index);
            }
        });

    })
}

var importBase64 = function(values, index){
    if(index == undefined || index == null){
        index = countConfiguration - 1;
    }

    $('#line_' + index).attr('typeFilter', "IMGB64");

    if(values == undefined || values == null){
        values = JSON.parse($('#config_0').val());
    }

    $('#typeData_' + index).val('IMGB64');

    specificDisplay($('#typeData_' + index));

    $('#line_' + index + ' #format').val(values.format);

    if(values.format == "RAW"){
        $('#line_' + index + ' #channel').val(values.channels);
    }
    else{
        $('#line_' + index + ' #channel').css('display', 'none');
    }

    $('#line_' + index + ' #width').val(values.width);
    $('#line_' + index + ' #height').val(values.height);

    if(!isListio){
        $('#line_' + index + ' #dataBase64_' + index).val(imgsB64[0]);
    }
    else{
        $('#line_' + index + ' #dataBase64_' + index).val(values.data);
    }

}

var importSqrCrop = function(values, indexLine){
    if(indexLine == undefined || indexLine == null){
        indexLine = countConfiguration - 1;
    }

    $('#line_' + indexLine).attr('typeFilter', "SQR_CROP");

    if(values == undefined || values == null){
        values = JSON.parse($('#config_0').val());
    }

    $('#typeData_' + indexLine).val('SQR_CROP');

    specificDisplay($('#typeData_' + indexLine));

    var index = 0;
    values.crop.forEach(function(value){
        if(index == 4){
            addCropLine(indexLine);
        }

        if((index % 4) == 0){
            index = 0;
        }

        $('#crop_line_' + indexLine + '_' + (cropLine - 1)).find('input')[index].value = value;
        index ++;
    });

    $('#line_' + indexLine + ' #pathInput').val(values.IMGF);
}

var importDefault = function(type, value, index){
    if(index == undefined || index == null){
        index = countConfiguration - 1;
    }

    $('#line_' + index).attr('typeFilter', type);

    if(value == undefined || value == null){
        value = JSON.parse($('#config_0').val());
    }

    $('#typeData_' + index).val(type);
    var input = $('#line_' + index).find('input');
    input.val(value.path);
}

var isDataSource = function(type){
    var regex = new RegExp('.*DataSource', 'g');

    return regex.test(type);
}