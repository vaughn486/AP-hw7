#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdbool.h>

#include "util.h"



int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];


/***********Veronica's testing space**************/




/*************************************************/



int handle_stdin() {
    /* TODO */
	return -1;
}

int handle_client_socket() {
    /* TODO */
	return -1;
}


int main(int argc, char **argv) {
    /* TODO */
        if(argc != 3){
                fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        	return EXIT_FAILURE;
	}
        //code from class code client.c
        //int client_socket, bytes_recvd, ip_conversion, retval = EXIT_SUCCESS;
        int ip_conversion;
        struct sockaddr_in server_addr;         //SERVER_ADDR
        socklen_t addrlen = sizeof(struct sockaddr_in);
        //char buf[BUFLEN];
        char *addr_str = argv[1];
        memset(&server_addr, 0, addrlen); // Zero out structure server_addr
        // Convert character string into a network address.
                //inet_pton converts the character string source to network address structure (in the AF address family) and copies it to destination
        //inet_pton(af, src, dst)   dst written in network byte order (big endian)
        ip_conversion = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
        //will tell us if ip conversion was successful
        if (ip_conversion == 0) {
                fprintf(stderr, "Error: Invalid IP address '%s'.\n", addr_str);
                return EXIT_FAILURE;
                //retval = EXIT_FAILURE;
                //goto EXIT;
        } else if (ip_conversion < 0) {
                fprintf(stderr, "Error: Failed to convert IP address. %s.\n", strerror(errno));
                return EXIT_FAILURE;
                //retval = EXIT_FAILURE;
                //goto EXIT;
        }

	//check port number
	//convert arg[2] to int -> atoi
	int portnum = atoi(argv[2]);
	if(portnum <1024 || portnum>65535){
		fprintf(stderr, "Error: Invalid input '%s' received for port number.\n", argv[2]);
		return EXIT_FAILURE;
	}

	printf("exited successfully\n");
	return EXIT_SUCCESS;
}

