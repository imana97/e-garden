var net = require('net');

var interpreter=function (message){
  console.log(message);


};

// Start a TCP Server
net.createServer(function (socket) {


  // Handle incoming messages from clients.
  socket.on('data', function (data) {
    interpreter(data.toString('ascii'),socket); // send it to interpreter.

    // there is a ping pong event. In this case, send the current time
    socket.write("1251;5;"+new Date().toLocaleString());
  });

  // Remove the client from the list when it leaves
  socket.on('end', function () {
  console.log('client disconnected');
});
  
}).listen(5000);

// The terminal message.
console.log("E-garden server is running on port 5000\n");
