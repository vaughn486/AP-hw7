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


int retval = EXIT_SUCCESS;

//phillip "everytime print to stdout, fflush"


int handle_stdin() {

	//clear outbuf before putting anything in
	memset(outbuf, '\0', MAX_MSG_LEN + 1);

	if (fgets(outbuf, sizeof(outbuf), stdin) == NULL) {

        	if (feof(stdin)){	//eof reached (ctrl d)
            		printf("\n");
			close(client_socket);
                        exit(EXIT_SUCCESS);
        	}
		printf("fgets error in stdin handler\n");
		fprintf(stderr, "Error: fgets() failed.\n");
                return EXIT_FAILURE;
        }

        //checks for newline in user input
        if (strchr(outbuf, '\n') != NULL){	//strchr searches for the 1st occurance of the \n
            	//removes new line
		if (outbuf[strlen(outbuf)-1] == '\n') {
        		outbuf[strlen(outbuf)-1] = '\0';
    	    	}

		//send the user's input
		//if the user's input was "bye", terminate client
         	if(strcmp(outbuf, "bye") == 0){
                	printf("Goodbye.\n");
			if (send(client_socket, outbuf, strlen(outbuf)+1, 0) < 0) {
                                fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
                                return EXIT_FAILURE;
                        }
			close(client_socket);
                	exit(EXIT_SUCCESS);

            	} else {
			//if there is nothing in the outbuf (input was just a new line)
			//then don't send to server
			if(strlen(outbuf) != 0){
				if (send(client_socket, outbuf, strlen(outbuf)+1, 0) < 0) {
                                	fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
                                	return EXIT_FAILURE;
                        	}
			}
            	}


        } else {	//if no newline, user input did not fit into buffer, is too long
            	printf("Sorry, limit your message to 1 line of at most %d characters.\n", MAX_MSG_LEN);
            	// consume remaining characters until EOF or new line is encountered
            	int c;
            	while (c != EOF && c != '\n'){
                	c = getchar();
            	}
        }
	return EXIT_SUCCESS;
}

//handles information recieved from server
int handle_client_socket() {

	//clear inbuf b4 putting anything in
	memset(inbuf, '\0', BUFLEN+1);

	int bytes_recvd = recv(client_socket, inbuf, BUFLEN, 0);


	if (bytes_recvd < 0 && errno != EINTR){
                fprintf(stderr, "\nWarning: Failed to receive incoming message. %s.\n", strerror(errno));
		return EXIT_SUCCESS;
	}
	if(bytes_recvd == 0){
		fprintf(stderr, "Connection to server has been lost.\n");
		return EXIT_FAILURE;
	}

        inbuf[bytes_recvd] = '\0';
	
	if (strcmp(inbuf, "bye") == 0) {
                printf("Server initiated shutdown.\n");
		close(client_socket);
		exit(EXIT_SUCCESS);
	} else{
		printf("%s\n", inbuf);
	}
	
	return EXIT_SUCCESS;
}


int main(int argc, char **argv) {
        if(argc != 3){
                fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        	return EXIT_FAILURE;
	}
        //code from class code client.c
        //int client_socket, bytes_recvd, ip_conversion, retval = EXIT_SUCCESS;
        int  bytes_recvd, ip_conversion;
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
	int portnum = atoi(argv[2]);
	if(portnum <1024 || portnum>65535){
		fprintf(stderr, "Error: Invalid input '%s' received for port number.\n", argv[2]);
		retval = EXIT_FAILURE;
		goto EXIT;
	}

	//prompting user for username
	char username[MAX_NAME_LEN+1] = "";
        char buffer[50];
        int balance = 0;

        while (balance == 0) {
            printf("Enter your username: ");
            fflush(stdout);
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
		fprintf(stderr, "Error: fgets() failed.\n");
                exit(EXIT_FAILURE);
            }
            // Remove newline character from buffer
            buffer[strcspn(buffer, "\n")] = 0; 
            if (strlen(buffer) > MAX_NAME_LEN) {
                printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
                strcpy(buffer, "");
                balance = 0;
            } else {
                // Copy buffer to username
                strcpy(username, buffer);
                balance = 1;
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
	
	//receiving welcome message (+connected users)
    	if ((bytes_recvd = recv(client_socket, inbuf, BUFLEN, 0)) < 0) {
        	fprintf(stderr, "Error: Failed to receive message from server. %s.\n",	strerror(errno));
        	retval = EXIT_FAILURE;
        	goto EXIT;
    	}

    	inbuf[bytes_recvd] = '\0';

	//for formatting of welcome message
	printf("\n");
    	printf("%s\n\n", inbuf);

	//clearing the inbuf
	memset(inbuf, '\0', BUFLEN + 1);

/******************************************************/



	//sending username to server
	sprintf(outbuf, "%s", username);
        outbuf[strlen(outbuf)] = '\0';
        if (send(client_socket, outbuf, strlen(outbuf)+1, 0) < 0) {
                fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
                retval = EXIT_FAILURE;
                goto EXIT;
        }

	//clearing the outbuf
	memset(outbuf, '\0', MAX_MSG_LEN + 1);


/******************************************/

//PART 4

	printf("[%s]: ", username);
	fflush(stdout);

	fd_set sockset;

	while(true){

		//note: client_socket just refers to int, that is the file descriptor for where the information flow is
		//max socket is our client socket
		//int max_socket = client_socket;

		//place sockets to be tested in bit set of type FD_SET
                //              use FD_SET to set the sockets in here

        	FD_ZERO(&sockset);      //clearing the sockset (making all of the bits = 0)
        	FD_SET(STDIN_FILENO, &sockset);
        	FD_SET(client_socket, &sockset);        //adding client_socket as a socket to watch
		//printf("file descriptor of client_socket is: '%d'\n", client_socket);	
	
		if(select(client_socket + 1, &sockset, NULL, NULL, NULL) < 0){
			//error
			fprintf(stderr, "Error: select() failed. %s.\n", strerror(errno));
			retval = EXIT_FAILURE;
			goto EXIT;
		}	

		if(FD_ISSET(client_socket, &sockset)){
			printf("\n");
                        if (handle_client_socket() == EXIT_FAILURE) {
                                retval = EXIT_FAILURE;
                                goto EXIT;
                        }
                }


		if(FD_ISSET(STDIN_FILENO, &sockset)){
			if (handle_stdin() == EXIT_FAILURE) {
                		retval = EXIT_FAILURE;
                		goto EXIT;
            		}
		}

		printf("[%s]: ", username);
		fflush(stdout);
	
	}
	

/*******************************************/


	goto EXIT;


EXIT:
	if (fcntl(client_socket, F_GETFD) != -1) {
        	close(client_socket);
    	}
	return retval;



}

