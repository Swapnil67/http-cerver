#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <setjmp.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "sv.h"

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


#define KILO 1024
#define MEGA (1024 * KILO)
#define GIGA (1024 * MEGA)

#define REQUEST_BUFFER_CAPACITY (640 * KILO)

char request_buffer[REQUEST_BUFFER_CAPACITY];

jmp_buf handle_request_error;

void http_error(int status_code, const char *message) {
    (void) status_code;
    fprintf(stderr, "%s\n", message);
    longjmp(handle_request_error, 1);    
}

void handle_request(int fd) {
    ssize_t request_buffer_size = read(fd, request_buffer, REQUEST_BUFFER_CAPACITY);
    if(request_buffer_size == 0) http_error(400, "EOF");
    if(request_buffer_size < 0) http_error(500, strerror(errno));

    printf("request_buffer_size: %lu\n", request_buffer_size);

    String_View buffer = {
	.len = (size_t)request_buffer_size,
	.data = request_buffer
    };

    String_View line = sv_chop_by_delim(&buffer, '\n');
    if(!line.len) {
	http_error(400, "Empty status line\n");
    }

    // String_View method = sv_chop_by_delim(&line, ' ');
    // printf("#%.*s#\n", (int)method.len, method.data);
    
    
    // while(buffer.len) {
    // 	string_view line = sv_chop_by_delim(&buffer, '\n');
    // 	line = sv_trim(line);
    // 	if(line.len) {
    // 	    // printf("len: %lu\n", line.len);
    // 	    printf("#%.*s#\n", (int)line.len, line.data);
    // 	}
    // }

    printf("\n");

    char newline = '\n';
    write(STDOUT_FILENO, &request_buffer, (size_t)request_buffer_size);
    write(STDOUT_FILENO, &newline, 1);
}


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
    
    
    // printf("Hello Cerver\n");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("server_fd: %d\n", server_fd);
    
    if(server_fd < 0) {
	fprintf(stderr, "Could not crate socket epicly %s\n", strerror(errno));
	exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    int enable = 1;
    int set_reuse_addr = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if(set_reuse_addr < 0) {
	fprintf(stderr, "Failed to set SO_REUSEADDR option\n");
	exit(1);
    }

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
	// printf("client_addr: %lu \n", sizeof(client_addr));	

	if(client_fd < 0) {
	    fprintf(stderr, "Could not accept connection. This is unacceptable! %s\n", strerror(errno));
	    exit(1);
	}
	assert(client_addrlen == sizeof(client_addr));

	if(setjmp(handle_request_error) == 0) {
	    handle_request(client_fd);
	}
	printf("----------------------------\n");
	
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


