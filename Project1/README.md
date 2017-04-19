# CS118 Project 1
Name: Caroline Quigg
UID: 604596183

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` and `client` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` to add your userid for the `.tar.gz` turn-in at the top of the file.

##server.cpp
Source code for server. The user specifies the port number that the service is hosted on and a directory for the transferred files to be saved in. The directory should exist before calling server. The server then accepts connections on the specified port. Once a connection is made, a thread is spawned which monitors the client, waiting for data to be sent and then saving it to a file. If the client doesn't end the TCP connection and fails to send data after 10 seconds, the server records an error. The server can support up to 10 simultaneous connections.

##client.cpp
Source code for client. The user specifies the name/IP and port number of the desired server to connect to. The user also specifies a file to send. The file should exist before calling client. If the client cannot connect to the server after 10 seconds it terminates the connection and records an error. Similarly, if the client is unable to write to the server after 10 seconds,an error is recorded and the connection is terminated. 

##Problems
The main issue I ran into was to figure out how to gracefully handle the timeout specifications. During discussion, my TA told us about the select() function, which can be used to check when a socket is ready to be read from/written to, as well as accepting a timeout value. I also had trouble figuring out how to test the timeout. Again my TA provided us with sample code using the tc tool to simulate package loss. 

##Outside Sources
I used the "Guide to Network Programming Using Sockets" provided on the project website:
http://beej.us/guide/bgnet/

