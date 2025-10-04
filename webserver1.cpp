#include <iostream>
#include <stdlib.h>
#include<fstream>
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
    	while(1)
    	{
 
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
		char buffer[4096];
		string request;

		//n is the number of bytes read
		int n = recv(newSd, buffer, sizeof(buffer), 0);
		if(n == 0 or n < 0) break; 	//client closed connection or error occured
		request.append(buffer, n); 	//put the first n bytes from buffer into a string to easily parse them

		//isolate the path line
		size_t method_end = request.find(' ');
		size_t path_end   = request.find(' ', method_end + 1);
		string path = request.substr(method_end + 1, path_end - method_end - 1);
		path = "." + path;
		cout << path << endl;

		//open the file 
		fstream file;
		file.open(path, ios::in | ios::binary);
            	if(file.is_open())
		{
                	cout<<"[LOG] : File is ready to Transmit.\n";
            	}
            	else
		{
                	cout<<"[ERROR] : File loading failed, Exititng.\n";
			string noHeader = "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found"; 
			send(newSd, noHeader.c_str(), noHeader.length(), 0);
    			close(newSd);
    			continue; // skip to the next client
            	}

		//setup headers
		string header;

		string extension;
		// find the last '.' in the filename
		size_t dotPos = path.find_last_of('.');
		//last dot found
		if (dotPos != std::string::npos) {
		    extension = path.substr(dotPos + 1);  
		} else {
		    extension = ""; // no extension found
		}

		if(extension == "html")      header = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
		else if(extension == "pdf")  header = "HTTP/1.1 200 OK\r\nContent-Type: application/pdf\r\n\r\n";
		else if(extension == "jpg")  header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";

		string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            	cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";
            	cout<<"[LOG] : Sending...\n";
		header += contents;
		int bytes_sent = send(newSd, header.c_str(), header.length(), 0);
            	cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";
            	cout<<"[LOG] : File Transfer Complete.\n";
		
		//after sending, close file and connection
		file.close();          
		close(newSd);        
    	}
}
