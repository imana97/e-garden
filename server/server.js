var net = require('net');

var interpereter=function (message){


};

// Start a TCP Server
net.createServer(function (socket) {
 
  // Send a nice welcome message and announce
  socket.write("Welcome E-garden");

  // Handle incoming messages from clients.
  socket.on('data', function (data) {
    console.log(data.toString('ascii'));
    socket.write(new Date().toISOString());
  });

  // Remove the client from the list when it leaves
  socket.on('end', function () {
  console.log('client disconnected');
});
  
}).listen(5000);

// The terminal message.
console.log("Chat server running at port 5000\n");
