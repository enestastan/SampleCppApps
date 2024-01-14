#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // socket()

#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <unistd.h> // read(), write(), close()
#include <errno.h>  // errno
#include <string.h> // strerror()

static const uint16_t PORT = 5050;

static const uint8_t MAX_BUFFER_SIZE = 100;
static const char* MESSAGE = "This is a message from server.";

int main()
{
    struct sockaddr_in serverAddr, clientAddr; 
    uint8_t uRxBuffer[MAX_BUFFER_SIZE];

	// socket create and verification 
	int serverFd = socket(AF_INET, SOCK_STREAM, 0); 
	if (serverFd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
    }

	bzero(&serverAddr, sizeof(serverAddr)); 

	// assign IP, PORT 
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serverAddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) != 0) { 
		printf("socket bind failed... errno : %d %s\n", errno, strerror(errno)); 
		exit(0); 
	} 
	else {
		printf("Socket successfully binded..\n"); 
    }

	// Now server is ready to listen and verification 
	if ((listen(serverFd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Server listening..\n"); 
    }

    socklen_t addrLength = sizeof(clientAddr);

	// Accept the data packet from client and verification 
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLength); 
	if (clientFd < 0) { 
		printf("server accept failed...\n"); 
		exit(0); 
	} 
	else {
		printf("server accepted the client...\n"); 
    }

    if( -1 == send(clientFd, MESSAGE, strlen(MESSAGE), 0)) {
        printf("Receive failed!");
        exit(0);
    }

    ssize_t nRecvBytes = recv(clientFd, uRxBuffer, MAX_BUFFER_SIZE, 0);
    if( -1 == nRecvBytes) {
        printf("Receive failed!");
        exit(0);
    }else {
        printf("Received num of bytes : %zd\n", nRecvBytes);
        printf("Received message : %s\n", uRxBuffer);
    }

	// After chatting close the socket 
	close(clientFd); 
	close(serverFd); 
    
    return 0;
}
