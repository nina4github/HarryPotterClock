var restify = require('restify')
	, fs = require('fs');

function respond(req, res, next) {
  //res.send('you are requesting info for tag: ' + req.params.name);
  	// here do the parsing of the name (req.params.name)
	// parse the tagstorage.txt file and return the corresponding json element
	console.log("request = "+ req.params.name);
	var value = parseTag(req.params.name,res);
	
}

var server = restify.createServer({name:'Tag Parser App'});
server.get('/tag/:name', respond);
server.head('/tag/:name', respond);

server.listen(8080, function() {
  console.log('%s listening at %s', server.name, server.url);
});


function parseTag(name,res){
	// open file and search for the value
	fs.readFile(__dirname + '/../tagstorage.txt', function (err, data) {
	  	if (err) {
		    res.writeHead(500);
			return res.end('Error loading storage file');
		}
		// storage (array of strings)
		var storage = data.toString().split("\n");
	
		// search value in storage
		var value = storage.filter(function (item){
				return JSON.parse(item).name == name; // convert item to JSON and confront name value
					});
	//	console.log("value: "+value);
		var isstored = value.length>0;
	//	console.log("isstored? "+isstored);
		if (isstored){
			res.writeHead(200);
			//return res.end(JSON.stringify(value));
			//return res.end(value.toString());
			console.log("result = "+ JSON.parse(value).text+","+JSON.parse(value).type);
			return res.end(JSON.parse(value).text+","+JSON.parse(value).type);
			//return value;	
		}else{
			res.writeHead(500);
			return res.end('Error tag not recognized');
		}
		
	});
	// if the value exists return tupla
}
