var send = require('send')
  , http = require('http')
  , url = require('url')
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
	send(req, url.parse(req.url).pathname)
	  .root(__dirname + '/../')
	  .pipe(resp);
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
	}).getUserTimeline(function (err, data) {
		 	console.log("timeline: "+data);
			socket.emit('twitter',data);
		}
	);
	stream.on('data',function(data){
		socket.emit('twitter_stream',data);
	});	
});
	
	
	