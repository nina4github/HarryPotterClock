var app = require('http').createServer(handler)
  , io = require('socket.io').listen(app)
  , twitter = require('ntwitter')
  , fs = require('fs')
  , twit = new twitter({
  consumer_key: 'XqEWPKcib9LYeV0BoHfCrA',
  consumer_secret: 'rVWbCfvQKi27smxxTeNsLaAsBJtNPmR70BwvPaEDY',
  access_token_key: '744944190-XztmpL4NCaE8mzIVPzSbCgQeDOU1CezXLwc2UMWY',
  access_token_secret: 'YRTwgF9QsQRqF7FBcEuhadfpTbBKKXbuhveBnGMZk'
});


app.listen(8000);

function handler (req, res) {
  fs.readFile(__dirname + '/index.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading index.html');
    }

    res.writeHead(200);
    res.end(data);
  });
}




io.sockets.on('connection', function (socket) {
	twit.stream('statuses/filter', {'follow':'744944190'},
		function(stream) {
	      stream.on('data',function(data){
	        socket.emit('twitter',data);
	      });
	    });
});
