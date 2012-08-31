var sys = require("sys"),
    http = require("http");
	
http.createServer(function(request, response) {
    response.writeHead(200, {'Content-Type': 'text/plain'});
	var texturl = request.url;
	var text = require('url').parse(texturl,true);
    response.write(text.query);
    response.end();
}).listen(8080);

sys.puts("Server running at http://localhost:8080/");