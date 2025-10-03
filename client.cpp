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
	//specify the ip and port we are trying to connect to
	if(argc != 3)
	{
		cerr << "Usage: ip_address port" << endl; exit(0);
	}
	char *serverIp = argv[1]; int port = atoi(argv[2]);

	//create a message buffer
	char msg[1500];
	    
	//grab host ip
	struct hostent* host = gethostbyname(serverIp);

	//setup the form our socket should take
	sockaddr_in sendSockAddr; 					//create the struct defining how to bind socket
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));		//zero out fields
	sendSockAddr.sin_family = AF_INET;				//set ipv4
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));	//set ip?
	sendSockAddr.sin_port = htons(port);			//set port 

	//create socket() with POSIX call
	int clientSd = socket(AF_INET, SOCK_STREAM, 0);		

	//try to connect...
	int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
	if(status < 0)
	{
		cout<<"Error connecting to socket!"<<endl;
		return -1;
	}
	cout << "Connected to the server!" << endl;
    
	//communication
	int bytesRead, bytesWritten = 0;
	while(1)
    	{
		cout << ">";
		string data;
		getline(cin, data);
		memset(&msg, 0, sizeof(msg));//clear the buffer
		strcpy(msg, data.c_str());
		if(data == "exit")
		{
		    send(clientSd, (char*)&msg, strlen(msg), 0);
		    break;
		}
		bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
		cout << "Awaiting server response..." << endl;
		memset(&msg, 0, sizeof(msg));//clear the buffer
		bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
		if(!strcmp(msg, "exit"))
		{
		    cout << "Server has quit the session" << endl;
		    break;
		}
		cout << "Server: " << msg << endl;
    	}
	close(clientSd);
	return 0;
}
