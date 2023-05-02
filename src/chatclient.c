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

void printprompt(){
	printf("[%s]: ", username);
}


int main(int argc, char **argv) {
        if(argc != 3){
                fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        	return EXIT_FAILURE;
	}
        //code from class code client.c
        //int client_socket, bytes_recvd, ip_conversion, retval = EXIT_SUCCESS;
        int  bytes_recvd, ip_conversion, retval = EXIT_SUCCESS;
        struct sockaddr_in server_addr;         //SERVER_ADDR
        socklen_t addrlen = sizeof(struct sockaddr_in);
        //char buf[BUFLEN];
        char *addr_str = argv[1];


	// Create a reliable, stream socket using TCP.
        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
                retval = EXIT_FAILURE;
                goto EXIT;
        }

        memset(&server_addr, 0, addrlen); // Zero out structure

/*********************************************************/	
	// Convert IP character string into IP network address.
        ip_conversion = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
        //will tell us if ip conversion was successful
        if (ip_conversion == 0) {
                fprintf(stderr, "Error: Invalid IP address '%s'.\n", addr_str);
                retval = EXIT_FAILURE;
                goto EXIT;
        } else if (ip_conversion < 0) {
                fprintf(stderr, "Error: Failed to convert IP address. %s.\n", strerror(errno));
                retval = EXIT_FAILURE;
                goto EXIT;
        }

	//check port number
	//convert arg[2] to int -> atoi
	int portnum = atoi(argv[2]);
	if(portnum <1024 || portnum>65535){
		fprintf(stderr, "Error: Invalid input '%s' received for port number.\n", argv[2]);
		retval = EXIT_FAILURE;
		goto EXIT;
	}


/********************************************************/


	//username prompting PART 2!!!!!
        /*      
        printf("Enter your username: ");
        strncpy(username, argv[3], strlen(argv[3]));
        printf("username: '%s'\n", username);
        */

	//hardcoding username for now
	//sprintf(username, "veronica");
	
	char username[MAX_NAME_LEN+1] = "";
	int balance = 0;
	printf("first check %ld", strlen(username));
        while (balance == 0) {
            printf("Please enter your username: ");
            fflush(stdout);
            fgets(username, MAX_NAME_LEN+1, stdin);
            username[strcspn(username, "\n")] = 0;
            //printf("max length is: %d\n", MAX_NAME_LEN);
	    printf("check after assigned %ld", strlen(username));
	    if (strlen(username) > MAX_NAME_LEN) {
                printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
                username[0] = '\0';
                balance = 0;
		printf("lencgth check in if statement %ld", strlen(username));
             }else{
	    	balance =1;
            }
        }
	printf("Hello, %s. Let's try to connect to the server.\n", username);


/***************************************************/

	server_addr.sin_family = AF_INET;   // Internet address family
    	server_addr.sin_port = htons(portnum); // Server port, 16 bits.
	//memset(buf, 0, BUFLEN);
	memset(inbuf, 0, BUFLEN + 1);
	memset(outbuf, 0, MAX_MSG_LEN + 1);


	//Connecting to the server
    	if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        	fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
        	retval = EXIT_FAILURE;
        	goto EXIT;
    	}

/****************************************************/
	
	//receiving welcome message
    	if ((bytes_recvd = recv(client_socket, inbuf, BUFLEN, 0)) < 0) {
        	fprintf(stderr, "Error: Failed to receive message from server. %s.\n",	strerror(errno));
        	retval = EXIT_FAILURE;
        	goto EXIT;
    	} else{
		printf("bytes were received\n");
	}

    	inbuf[bytes_recvd] = '\0';

	//for formatting of welcome message
	printf("\n");
    	printf("%s\n", inbuf);
	printf("\n\n");
/******************************************************/

	memset(inbuf, 0, BUFLEN + 1);
        memset(outbuf, 0, MAX_MSG_LEN + 1);

	printf("username: '%s'\n", username);

	//sending username to server
	sprintf(outbuf, "%s", username);
	//sprintf(outbuf, "Sending message to server at [%s:%d].\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        outbuf[strlen(outbuf)] = '\0';
        if (send(client_socket, outbuf, strlen(outbuf), 0) < 0) {
                fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
                retval = EXIT_FAILURE;
                goto EXIT;
        }










	goto EXIT;


EXIT:
	if (fcntl(client_socket, F_GETFD) != -1) {
        	close(client_socket);
    	}
	return retval;



}

