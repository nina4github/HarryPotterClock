var send = require('send')
  , http = require('http')
  , url = require('url')

  , sio = require('socket.io');
  //, twitter = require('ntwitter')
	
var util = require('util'),
    twitter = require('twitter');

	
var config = require('./twitterconfig.json');

var twit = new twitter({
  consumer_key: config.consumer_key,
  consumer_secret: config.consumer_secret,
  access_token_key:  config.access_token_key,
  access_token_secret: config.access_token_secret
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
	
	// this if is not working
	if(url.parse(req.url)==='/db.html'){
		console.log(req.url.pathname);
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
		//console.log(s);
		app.listen(8000);
	});


io.sockets.on('connection', function (socket) {
	twit.verifyCredentials(function (err, data) {
		//console.log(data);
	}).getUserTimeline({'include_entities':'true'},function (data, err) {
		 console.log("timeline: ",data,err);
			socket.emit('twitter',data);	
		}
	);

	// get('/statuses/show/744944190.json', {include_entities:true}, function(data, err) {
 //    	//console.log(util.inspect(data));
 //    	//console.log("timeline: ",data,err);
	// 	socket.emit('twitter',data);
	// });
	

	stream.on('data',function(data){
		socket.emit('twitter_stream',data);
	});	
});
	
	
	