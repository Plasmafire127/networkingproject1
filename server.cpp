#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cerr << "Usage: port" << endl;
		exit(0);
	}

	int port = atoi(argv[1]);
	//buffer to send and receive messages with
    	char msg[1500];

	//Describe the form we want our welcome socket to take
	//read sin as "socket internet"
	sockaddr_in servAddr;				//create the struct defining how to bind our socket
	bzero((char*)&servAddr, sizeof(servAddr)); 	//clear whole struct to zero
	servAddr.sin_family = AF_INET; 			//Set to use IPV4 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 	//bind to all local interfaces (accept a request from anywhere)
	servAddr.sin_port = htons(port); 		//set port number

	//using POSIX socket() call to create a socket object. serverSd is just a file descriptor to refer to it.  
    	int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    	if(serverSd < 0)
    	{
        	cerr << "Error establishing the server socket" << endl;
        	exit(0);
    	}

	//bind the socket to its local address using the previously defined settings
    	int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    	if(bindStatus < 0)
    	{
        	cerr << "Error binding socket to local address" << endl;
        	exit(0);
    	}
    	cout << "Waiting for a client to connect..." << endl;

	//start listening and backlog/queue up to 5 connections
	listen(serverSd, 5);

    	//create a new address to connect with the client (other details like ip are filled at runtime when client connects)
    	sockaddr_in newSockAddr;
    	socklen_t newSockAddrSize = sizeof(newSockAddr);
	//create a new socket descriptor for the client using accept() 
    	int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    	if(newSd < 0)
    	{
        	cerr << "Error accepting request from client!" << endl;
        	exit(1);
    	}
    	cout << "Connected with client!" << endl;

	//communication between server and client
	int bytesRead, bytesWritten = 0;
    	while(1)
    	{
        	//receive a message from the client (listen)
        	cout << "Awaiting client response..." << endl;
		memset(&msg, 0, sizeof(msg));//clear the buffer
		bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
		if(!strcmp(msg, "exit"))
		{
		    cout << "Client has quit the session" << endl;
		    break;
		}
		cout << "Client: " << msg << endl;
		cout << ">";
		string data;
		getline(cin, data);
		memset(&msg, 0, sizeof(msg)); //clear the buffer
		strcpy(msg, data.c_str());
		if(data == "exit")
		{
		    //send to the client that server has closed the connection
		    send(newSd, (char*)&msg, strlen(msg), 0);
		    break;
		}
		//send the message to client
		bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    	}
}
