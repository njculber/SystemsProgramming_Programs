/*
	Neil Culbertson
	Systems Programming
	Assignment 5: File Server
	Dr. Scott Kuhl
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char password[] = "this";
/* We will use getaddrinfo() to get an addrinfo struct which will
 * contain information the information we need to create a TCP
 * stream socket. The caller should use
 * freeaddrinfo(returnedValue) when the returend addrinfo struct
 * is no longer needed. */
struct addrinfo* example_getaddrinfo(const char* hostname, const char* port)
{
	/* First we need to create a struct that tells getaddrinfo() what
	 * we are interested in: */
	struct addrinfo hints; // a struct 
	memset(&hints, 0, sizeof(hints)); // Set bytes in hints struct to 0
	hints.ai_family = AF_INET;    // AF_INET for IPv4, AF_INET6 for IPv6, AF_UNSPEC for either
	hints.ai_socktype = SOCK_STREAM;  // Use TCP

	// hints.ai_flags is ignored if hostname is not NULL. If hostname
	// is NULL, this indicates to getaddrinfo that we want to create a
	// server.
	hints.ai_flags = AI_PASSIVE; 
	struct addrinfo *result;  // A place to store info getaddrinfo() provides to us.
	
	/* Now, call getaddrinfo() and check for error: */
	int gai_ret = getaddrinfo(hostname, port, &hints, &result);
	if(gai_ret != 0)
	{
		// Use gai_strerror() instead of perror():
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_ret));
		exit(EXIT_FAILURE);
	}
	return result;
}

/* Create a socket. Returns a file descriptor. */
int example_socket(struct addrinfo *ai)
{
	int sock = socket(ai->ai_family,
	                  ai->ai_socktype,
	                  ai->ai_protocol);
	if(sock == -1)
	{
		perror("socket() error:");
		exit(EXIT_FAILURE);
	}
	return sock;
}

void example_connect(int sock, struct addrinfo *ai)
{
	if(connect(sock, ai->ai_addr, ai->ai_addrlen) == -1)
	{
		perror("connect() failure: ");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	char *hostname = argv[1];
	char *port = argv[2]; // can also be a name in /etc/services
	//char *command = argv[3];
	//char *fileName = argv[4];
	ssize_t recvdBytes = 0;

	// getaddrinfo(): network address and service translation:
	struct addrinfo *socketaddrinfo = example_getaddrinfo(hostname, port);

	// socket(): create an endpoint for communication
	int sock = example_socket(socketaddrinfo);
	printf("Connecting to server\n");
	// connect(): initiate a connection on a socket
	example_connect(sock, socketaddrinfo);

	// enter password
	//char password[100];
	char recvbuf[100];
	//fgets(password, 100, stdin);

	

	
	printf("Sending password\n");
	if(send(sock, password, strlen(password), 0) == -1){
		
		perror("send");
		exit(EXIT_FAILURE);
	}
	printf("Connected\n");
	if( (recvdBytes = recv(sock, recvbuf, 1024, 0)) == -1){
		perror("recv");
	}
	else{
		recvbuf[recvdBytes] = '\0';
		printf("%s\n", recvbuf);
	}

	/*----------------------------SENDING COMMAND AND FILE TO SERVER-----------------------------*/
	// send commmand to server if command is there
	printf("Sending %s command \n", argv[3]);
	if(argv[3]){
		if(send(sock, argv[3], strlen(argv[3]), 0) == -1){
			perror("send");
			exit(EXIT_FAILURE);
		}
	}
	
	// Send file argument to the server
	if(argv[4]){
		//int bytes;
		
		//printf("Size of buf: %d\n", (int)sizeof(buf));
		
		printf("Sending file\n");
		if(send(sock, argv[4], strlen(argv[4]), 0) == -1){
			perror("send");
			exit(EXIT_FAILURE);
		}
	}/*
	char buf[1];
	FILE *fp = fopen(argv[4], "rb");
	if(!fp){
			perror("fopen");
			return -1;
	}
	while(!feof(fp)){
		//bytes = 
		fread(buf, 1, 1, fp);
		buf[1] = '\0';
		//printf("Read: %d bytes, result: %s\n", bytes, buf);
		if(send(sock, buf, strlen(buf), 0) == -1){
			perror("Send");
			return 1;
		}
	}*/
	freeaddrinfo(socketaddrinfo); // done with socketaddrinfo
	while(1)
	{
		//char recvbuf[1024];
		ssize_t recvdBytes = 0;

		/* Read up to 1024 bytes from the socket. Even if we know that
		   there are far more than 1024 bytes sent to us on this
		   socket, recv() will only give us the bytes that the OS can
		   provide us. If there are no bytes for us to read (and the
		   connection is open and without error), recv() will block
		   until there are bytes available.
		   
		   Note that it is possible to make recv() non-blocking with
		   fcntl(). */
		recvdBytes = recv(sock, recvbuf, 1024, 0);
		if(recvdBytes > 0){ // print bytes we received to console
			//fwrite(recvbuf, 1, recvdBytes, stdout);
			recvbuf[recvdBytes] = '\0';
			printf("%s\n", recvbuf);
		}
		else if(recvdBytes == 0) // server closed connection
		{
			fflush(stdout);
			close(sock);
			exit(EXIT_SUCCESS);
		}
		else
		{   // error receiving bytes
			perror("recv:");
			close(sock);
			exit(EXIT_FAILURE);
		}
	}
}