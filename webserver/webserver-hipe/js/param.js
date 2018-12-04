$(function () {

    var wto;
    $('#attr_server_url').focus();
    $('#attr_server_url').select();

    $('#validatedParam').on('click', function (ev) {
        var srvUrl = $('#attr_server_url').val();
        var version = $('#attr_version').val();

        var isHttp = new RegExp('^http').test(srvUrl);
        var isHttps = new RegExp('^https').test(srvUrl);
        if(!isHttp && ! isHttps){
            srvUrl = 'http://' + srvUrl;
        }

        sessionStorage.setItem('srvUrl', srvUrl);
        sessionStorage.setItem('version', version);

        notifySuccess('L\'élément a bien été enregistré');

        window.location.pathname = '/model';
    });
    $('#attr_server_url').on('focusout',
        function() {
            clearTimeout(wto);
            tryConnect();
        });

    function tryConnect() {
        var width = $('.card').width();
        var height = $('.card').height();

        $('.card').prepend('<div id="spinnerContent" style="width: ' + width + 'px;height: ' + height + 'px;position: absolute;' +
            'background-color: rgba(146, 146, 146, 0.5);z-index: 1;">' +
            '<div class="loader" style="position: relative;left: 50%; top: 50%;z-index: 2;"></div></div>')

        var filterBodyRaw = {
            "name": "Version",

            "command":
            {
                "type": "Version"
            }
        };

        var srvUrl = $('#attr_server_url').val();

        var isHttp = new RegExp('^http').test(srvUrl);
        var isHttps = new RegExp('^https').test(srvUrl);
        if(!isHttp && ! isHttps){
            srvUrl = 'http://' + srvUrl;
        }

        $.ajax({
            data: JSON.stringify(filterBodyRaw),
            url: srvUrl + '/json',
            type: 'POST',
            dataType: 'JSON',
            error: function (xhr, status, error) {
                notifyError('Le serveur n\'a pas retourné de numéro de version. Verifiez qu\'un serveur Hipe s\'y trouve.')
                $('#spinnerContent').remove();
            },
            success: function (data) {
                var versionSrv = data;

                $('#attr_version').val(versionSrv.Version);
                $('#validatedParam').attr('disabled', false);

                $('#spinnerContent').remove();
            }

        });
    }


    $('#attr_server_url').on('change',
        function() {
            clearTimeout(wto);
            wto = setTimeout(function () {
                tryConnect();
            }, 2000);

            tryConnect();
        });

    if ((sessionStorage.getItem('srvUrl') != undefined && sessionStorage.getItem('version') != undefined) && window.location.pathname == "/") {
        window.location.pathname = '/model';
    }

    if ((sessionStorage.getItem('srvUrl') != undefined && sessionStorage.getItem('version') != undefined) && window.location.pathname == "/parameters") {
        $('#attr_server_url').val(sessionStorage.getItem('srvUrl'));
        $('#attr_version').val(sessionStorage.getItem('version'));
    }

    if ($('#attr_version').val() == "") {
        $('#validatedParam').attr('disabled', 'disabled');
    }

    if ($('#attr_server_url').val() == "") {
        $('#attr_server_url').val('https://webhipe.aneo.fr:9133');
        //$('#attr_server_url').click();
    }
});