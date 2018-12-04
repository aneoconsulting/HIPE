var express = require('express');
var app = express();
var fs = require('fs');
var path = require("path");
var bodyParser = require('body-parser');
var filtersType = require('./filters.json');
var datas = require('./data.json');
var orm = require("orm");
var $ = require('jquery');

var key = fs.readFileSync('C:/xampp/cert/https_aneo.key');
var cert = fs.readFileSync( 'C:/xampp/cert/https_aneo.crt' );
//var ca = fs.readFileSync( 'encryption/intermediate.crt' );
var https = require('https');

var options = {
    key: key,
    cert: cert
};

app.use(orm.express("mysql://root:@localhost:4406/hipe", {
    define: function (db, models, next) {
        models.models = db.define("models", {
            version: String,
            name: String,
            date_creation: Date,
            json: Object,
            id_of_next_element: Number
        });

        models.points = db.define("points", {
            selector: String,
            position_x: Number,
            position_y: Number,
            model_id: Number
        });

        next();
    }
}));

app.set('view engine', 'ejs');
app.use(bodyParser.json({type: 'application/json', limit: '50mb'}));
app.use(bodyParser.urlencoded({limit: '50mb', extended: true}));

app.use('/css', express.static('dist/css'));
app.use('/js', express.static('dist/js'));
app.use('/fonts', express.static('dist/fonts'));
app.use('/images', express.static('images'));
app.use('/node_modules', express.static('node_modules'));

app.get('/', function (req, res) {
    res.render('index.ejs');
});

app.get('/parameters', function (req, res) {
    res.render('parameters.ejs');
});

app.get('/model', function (req, res) {
    res.render('graph.ejs');
});


app.get('/model/:id', function (req, res) {
    req.models.models.find({id: req.params.id}, function (err, model) {
        res.render('graph.ejs', {model: model});
    });
});


app.get('/point/:id/:selector', function (req, res) {
    req.models.points.find({model_id: req.params.id, selector: req.params.selector}, function (err, model) {
        res.send(JSON.stringify(model));
    });
});

app.post('/model', function (req, res) {
    var newRecord = {};
    newRecord.version = req.body['version'];
    newRecord.name = req.body['name'];
    newRecord.json = req.body['json'];
    newRecord.date_creation = new Date();
    newRecord.id_of_next_element = 0;

    req.models.models.create(newRecord, function (err, model) {
        res.send({'id': model.id});
    });
});

app.put('/model/:id', function (req, res) {
    req.models.models.find({id: req.params.id}, function (err, model) {
        model = model[0];

        model.version = req.body['version'];
        model.name = req.body['name'];
        model.json = req.body['json'];
        model.id_of_next_element = req.body['id_of_next_element'];
        model.date_creation = new Date();

        var points = req.body['points'];

        var pointToSave = [];

            points.forEach(function (point) {
                console.log(point.selector);
                req.models.points.find({selector: point.selector, model_id: model.id}, 1, function (err, pointO) {

                    if (pointO != undefined && pointO.length > 0) {
                        pointO = pointO[0];

                        pointO.position_x = point.position_x;
                        pointO.position_y = point.position_y;


                        pointO.save();
                    }
                    else {
                        var pointO = {};

                        pointO.selector = point.selector;
                        pointO.model_id = model.id;
                        pointO.position_x = point.position_x;
                        pointO.position_y = point.position_y;

                        req.models.points.create(pointO,
                            function(err, items) {

                            });
                    }
                });
            });


        model.save(function (err) {
            res.send({'id': model.id});
        });
    });
});

app.delete('/model/:id', function (req, res) {
    req.models.models.find({id: req.params.id}, function (err, model) {
        model = model[0];

        if (model) {
            model.remove();
        }
    });
});

app.get('/models', function (req, res) {
    req.models.models.find().order('-date_creation').all(function (err, models) {
        res.render('models.ejs', {models: models});
    });
});

app.get('/filtersTypes', function (req, res) {
    res.status(200).json(filtersType);
});

app.get('/datas', function (req, res) {
    res.status(200).json(datas);
});

app.post('/filters', function (req, res) {
    db.push("/" + req.body['data'], req.body['data']);
    res.send();
});

https.createServer(options, app).listen(3000, function() {
    console.log('Example app listening on port 3000!');
});

//app.listen(3000, function () {
//    console.log('Example app listening on port 3000!');
//});

