#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

char response[] =
"HTTP/1.1 200 OK\n"
"Content-Type: text/html\n"
"\n"
"<html>"
"<head>"
"<title>PHP is the BEST!</title>"
"</head>"
"<body>"
"<h3>PHP is the BEST!</h3>"
"</body>"
"</html>\n";


int main(int argc, char *argv[]) {

    if(argc < 2) {
	fprintf(stderr, "Usage: nodec <port>\n");
	exit(1);
    }

    char *endptr;
    char *str = argv[1];
    int base = 10;
    uint32_t port = (uint32_t) strtoul(str, &endptr, base);
    if(endptr == str) {
	fprintf(stderr, "Invalid port.\n");
	exit(1);
    }
    
    
    printf("Hello Cerver\n");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("server_fd: %d\n", server_fd);
    
    if(server_fd < 0) {
	fprintf(stderr, "Could not crate socket epicly %s\n", strerror(errno));
	exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    //server_addr.sin_family = AF_INET;
    
    // * htons
    // * Big Endian: MSB is stored first
    // * Little Endian: LSB is stored first
    // * Converts from 'host byte order' to 'network byte order'
    // * Network Byte Order ==> Big Endian

    server_addr.sin_port = htons((uint16_t)port);
    int err = bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(err != 0) {
	fprintf(stderr, "Cound not bind socket. %s\n", strerror(errno));
	exit(1);
    }

    // * Listen for connections
    err = listen(server_fd, 69);
    if(err != 0) {
	fprintf(stderr, "Could not listen to socket, it's too quiet: %s\n", strerror(errno));
	exit(1);
    }

    // * Keep accepting the connections
    for(;;) {
	struct sockaddr_in client_addr;
	socklen_t client_addrlen = 0;
	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
	// printf("client_fd: %d\n", client_fd);

	if(client_fd < 0) {
	    fprintf(stderr, "Could not accept connection. This is unacceptable! %s\n", strerror(errno));
	    exit(1);
	}

	// printf("client_addr: %lu \n", sizeof(client_addr));
	assert(client_addrlen == sizeof(client_addr));


	// * Write dummy response
	// printf("response: %lu \n", sizeof(response));
	ssize_t err = write(client_fd, response, sizeof(response));
	if(err < 0) {
	    fprintf(stderr, "Could not send data: %s\n", strerror(errno));
	}

	// * Close the client connection
	err = close(client_fd);
	if(err < 0) {
	    fprintf(stderr, "Could not close client connection: %s\n", strerror(errno));
	}	
    }
    
    return 0;
}


