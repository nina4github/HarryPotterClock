var send = require('send')
  , http = require('http')
  , url = require('url')
  , querystring = require('querystring')
//express = require('express')
//  ,	app = express() //= require('http').createServer(handler)
//  , routes = require('./routes')
  , sio = require('socket.io') //.listen(app)
  , twitter = require('ntwitter')
  , fs = require('fs')
  , twit = new twitter({
  consumer_key: 'XqEWPKcib9LYeV0BoHfCrA',
  consumer_secret: 'rVWbCfvQKi27smxxTeNsLaAsBJtNPmR70BwvPaEDY',
  access_token_key: '744944190-XztmpL4NCaE8mzIVPzSbCgQeDOU1CezXLwc2UMWY',
  access_token_secret: 'YRTwgF9QsQRqF7FBcEuhadfpTbBKKXbuhveBnGMZk'
});

// 
// function handler (req, res) {
//   //fs.readFile(__dirname + '/index.html',
//   fs.readFile(__dirname + '/../idemolabfluid.html',
//   //fs.readFile('../idemolabfluid.html',
//   function (err, data) {
//     if (err) {
//       res.writeHead(500);
//       return res.end('Error loading index.html');
//     }
// 
//     res.writeHead(200);
//     res.end(data);
//   });
// };

var app = http.createServer( function(req,resp) {
	console.log(req.url);
	if(url.parse(req.url).path.indexOf('/store')>-1){
		filesystemstuff(resp);
	}
	if(url.parse(req.url).path.indexOf('/db.html')>-1){
				
				var queryObj = querystring.parse(url.parse(req.url).query)
				var parsingresponse = tagparser(queryObj)
				resp.writeHead(200);
				resp.end(JSON.stringify(parsingresponse));
	}
	else{
				send(req, url.parse(req.url).pathname)
				  .root(__dirname + '/../')
				  .pipe(resp);
	}
} );
var io = sio.listen(app);

var stream = null;

twit.stream('statuses/filter', {'follow':'744944190'},
	function(s) {
		stream = s;
		console.log(s);
		app.listen(8000);
	});


io.sockets.on('connection', function (socket) {
	twit.verifyCredentials(function (err, data) {
		//console.log(data);
	}).getUserTimeline({'include_entities':'true'},function (err, data) {
		 	console.log("timeline: "+data);
			socket.emit('twitter',data);
		}
	);
	stream.on('data',function(data){
		socket.emit('twitter_stream',data);
	});	
});
	
	
// the tagparser takes an object as input 
// the object has a key: "tag" and a value the tag string
// the function parses the value of the tag and returns a json object with the details associated to that tag
// the details are id, text, type = [actor|verb|object|context|indirect object]
// the type follows the definition of the key elements of an activity stream
function tagparser(tag){
	console.log(tag);
	var value = tag.tag;
	// tmp db of tags and related values
	var storedtags = [
						{"name":'aabbccss00',"text":'@vanessa',"type":"actor"},
						{"name":'aabbccss01',"text":'@morten',"type":"actor"},
						
						{"name":'aabbccdd00',"text":'#coffee',"type":"verb"},
						{"name":'aabbccdd01',"text":'#meeting',"type":"verb"},
						
						{"name":'aabbccee00',"text":'#projectA',"type":"object"},
						{"name":'aabbccee01',"text":'#projectB',"type":"object"},
						];
						
	var tupla = storedtags.filter(function(item){
		return item.name == value;
	})[0];
	console.log(tupla);
	return tupla;

	// here do your stuff
	// var objToJson = {"name":'aabbccss00',"text":'@vanessa'};
	// 	console.log(JSON.stringify(objToJson));
	
}	
	
function filesystemstuff(res){
	
	// READ FILE TO ARRAY
	// example of how to read the file and manipulate it through an array of JSON strings :)
	fs.readFile(__dirname + '/../tagstorage.txt', function (err, data) {
	  	if (err) {
		    res.writeHead(500);
			return res.end('Error loading tagstorage.txt');
		}
		var tmp = data.toString().split("\n");
	
		// search value in tmp (array of strings)
		var isstored = tmp.filter(function (item){
						return JSON.parse(item).name == 'aabbccee00'; // convert item to JSON and confront name value
					}).length>0;
					console.log("isstored? "+isstored);
		// res.writeHead(200);
		// return res.end(data);
	});
	
	// APPEND NEW TUPLA TO FILE
	// var tupla = {"name":"aabbccee02","text":"#projectB","type":"object"};
	// 		// append file 
	// 		fs.appendFile(__dirname + '/../tagstorage.txt', "\n"+JSON.stringify(tupla), function (err) {
	// 		  if (err) throw err;
	// 		  console.log('The "data to append" was appended to file!');
	// 		});
	// 	
	
	// READ FROM FILE
	// fs.readFile(__dirname + '/../tagstorage.txt', function (err, data) {
	//   	if (err) {
	// 	     res.writeHead(500);
	// 		 return res.end('Error loading tagstorage.txt');
	// 	}
	// 	
	// 	console.log("data: "+data);
	// 	res.writeHead(200);
	// 	return res.end(data);
	// });
}