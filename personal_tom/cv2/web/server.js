// server.js
var express = require('express');  
var app = express();  
var server = require('http').createServer(app); 
var io = require('socket.io')(server); 

//keep track of how times clients have clicked the button
var clickCount = 0;
var temp = 0.3;
var clientCount = 0;

app.use(express.static(__dirname + '/public')); 
//redirect / to our index.html file
app.get('/', function(req, res,next) {  
    res.sendFile(__dirname + '/public/RC_index.html');
});

io.on('connection', function(client) { 
    	  clientCount++;
	console.log('Client connected...', clientCount); 
	
	//When the server receives clicked message, do this
    client.on('clicked', function(data) {
		  //send a message to ALL connected clients
		  client.broadcast.emit('c++ request',data);
		  console.log(data);
    });
	
	//On client disconnection
	client.on('disconnect', function() {
		--clientCount;
	console.log('Client left. Remaining: ', clientCount);
	});
		
	//Receive toggle mode command, send to c++
	client.on('ToggleMode', function(data) {
		console.log('*** Toggle mode ***');
		console.log(data);
		//send a message to ALL connected clients (to c++)
		client.broadcast.emit('ToggleMode', data);
    });

    client.on('ToggleCamMode', function(data) {
		console.log('*** Toggle camera ***');
		console.log(data);
		//send a message to ALL connected clients (to c++)
		client.broadcast.emit('ToggleCamMode', data);
    });
	
	client.on('XboxMode', function(data) {
		console.log('*** Toggle Xbox mode ***');
		console.log(data);
		//send a message to ALL connected clients (to c++)
		client.broadcast.emit('XboxMode', data);
    });

	client.on('image', function(data) {
		//console.log('*** Image received (server) ***');
		//console.log(data);
		//console.log(data.image);
		//send a message to ALL connected clients (to c++)
		client.broadcast.emit('image', data);
    });

	client.on('PID', function(data) {
		console.log('*** Received PID data ***');
		console.log(data);
		//send a message to ALL connected clients (to c++)
		client.broadcast.emit('PID', data);
    });
});

//start our web server and socket.io server listening
server.listen(3000, function(){
  console.log('listening on *:3000');
}); 