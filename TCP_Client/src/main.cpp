#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // socket()
#include <arpa/inet.h>  // inet_addr()

#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <unistd.h> // read(), write(), close()
#include <errno.h>  // errno
#include <string.h> // strerror()

static const uint16_t PORT = 5050;
static constexpr char* IP_ADDRESS = "127.0.0.1";

static const uint8_t MAX_BUFFER_SIZE = 100;
static const char* MESSAGE = "This is a message from client.";

using namespace std;


// Driver function 
int main() 
{ 
	struct sockaddr_in servaddr; 
    uint8_t uRxBuffer[MAX_BUFFER_SIZE];

	// socket create and verification 
	int clientFd = socket(AF_INET, SOCK_STREAM, 0); 
	if (clientFd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
    }

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
	servaddr.sin_port = htons(PORT); 

    socklen_t length = sizeof(servaddr);

	// Accept the data packet from client and verification 
	
	if (-1 == connect(clientFd, (struct sockaddr*)&servaddr, length)) { 
		printf("client connect failed...\n"); 
		exit(0); 
	} 
	else {
		printf("client connected to server\n"); 
    }

    ssize_t nRecvBytes = recv(clientFd, uRxBuffer, MAX_BUFFER_SIZE, 0);
    if(-1 == nRecvBytes ) {
        printf("Receive failed!");
        exit(0);
    }else {
        printf("Received num of bytes : %d\n", nRecvBytes);
        printf("Received message : %s\n", uRxBuffer);
    }

    if( -1 == send(clientFd, MESSAGE, strlen(MESSAGE), 0)) {
        printf("Send failed!");
        exit(0);
    }
	// After chatting close the socket 
	close(clientFd); 
}

