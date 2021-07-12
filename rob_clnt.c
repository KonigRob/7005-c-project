/*-----------------------------------------------------------------------------
 * SOURCE FILE:    rob_clnt.c
 *
 * PROGRAM:        Assignment #1-FT
 *
 * FUNCTIONS:      <function headers>
				   int main(int argc, char** argv)
 *
 * DATE:           October 3th, 2020
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * NOTES:
 * The program will establish a TCP connection to a user specifed server.
 * The server can be specified using a fully qualified domain name or and
 * IP address.  AFter the connection has been established the user will be 
 * prompted to enter GET or SEND followed by a 4 digit port other than 7005
 * followed by the file they want to be tranferred.  The file is either fetched
 * from the server to the client, or sent to the server from the client. 
* --------------------------------------------------------------------------*/
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#define SERVER_TCP_PORT		7005	// Default port
#define BUFLEN			80  	// Buffer length
#define TRUE	1


/*--------------------------------------------------------------------------
 * FUNCTION:       sendFile
 *
 * DATE:           October 7th, 2020
 *
 * REVISIONS:      N/A 
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        void
 *
 * NOTES:
 * Looping through a buffer to read data off a file and send it across the 
 * connected socket.
 * -----------------------------------------------------------------------*/
void sendFile(unsigned char *buffer, size_t bytesToRead, FILE *file, int socket);

/*--------------------------------------------------------------------------
 * FUNCTION:       writeFile
 *
 * DATE:           October 7th, 2020
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        void
 *
 * NOTES:
 * Looping through a buffer of incoming data to write data on a file.
 * -----------------------------------------------------------------------*/
void writeFile(unsigned char *buffer, size_t bytesToRead, FILE *file, int socket);

/*--------------------------------------------------------------------------
 * FUNCTION:       initializeSecondSocket
 *
 * DATE:           October 7th, 2020
 *
 * REVISIONS:      N/A 
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        void
 *
 * NOTES:
 * Sets up a different socket connection with the client on the user specified
 * port, then opens/creates a file if possible to be read/written onto.
 * -----------------------------------------------------------------------*/
void initializeSecondSocket(char *sbuf, char *fileOpening, size_t length, size_t size, size_t fileLength);

/*--------------------------------------------------------------------------
 * FUNCTION:       getPort
 *
 * DATE:           October 7th, 2020
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        int
 *
 * NOTES:
 * Parses a char buffer to return the port number.
 * -----------------------------------------------------------------------*/
int getPort(char *buf, size_t length, size_t size);

/*--------------------------------------------------------------------------
 * FUNCTION:       getFilepath
 *
 * DATE:           October 7th, 2020
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        char pointer
 *
 * NOTES:
 * Parses a char buffer to return the filepath.
 * -----------------------------------------------------------------------*/
char* getFilepath(char *buf, size_t length);

/*--------------------------------------------------------------------------
 * FUNCTION:       main
 *
 * DATE:           October 3th, 2020
 *
 * REVISIONS:      N/A 
 *
 * DESIGNER:       Rob Konig
 *
 * PROGRAMMER:     Rob Konig
 *
 * INTERFACE:      <function header>
				   int main(int argc, char** argv)
 *
 * RETURNS:        <function return type>
 *
 * NOTES:
 * Establishes a connection with a specified server using an IP address on port
 * 7005.  Then prompts for user input, to have the user enter SEND or GET.
 * -----------------------------------------------------------------------*/
int main (int argc, char **argv){
	int sd, port, check_command = 0;
	struct hostent *hp;
	
	struct sockaddr_in server;
	char  *host, sbuf[BUFLEN], **pptr;
	char str[16];

	switch(argc){
		case 2:
			host =	argv[1];	// Host name
			port =	SERVER_TCP_PORT;
		break;
		case 3:
			host =	argv[1];
			port =	atoi(argv[2]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
			exit(1);
	}

	// Create the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Cannot create socket");
		exit(1);
	}
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL){
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

	// Connecting to the server
	if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't connect to server\n");
		perror("connect");
		exit(1);
	}
	printf("Connected:    Server Name: %s\n", hp->h_name);
	pptr = hp->h_addr_list;
	printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));
	printf("Transmit:\n");
	//gets(sbuf); // get user's text
	fgets (sbuf, BUFLEN, stdin);
	char get_Command[4];
	strncpy(get_Command, sbuf, 3);
	if(strcmp(get_Command, "GET") == 0){
		//then command entered was GET
		check_command = 1;
		send (sd, sbuf, BUFLEN, 0);
		char fileOpening[] = "wb";
		initializeSecondSocket(sbuf, fileOpening, 4, 4, 9);
	}
	char sent_Command[5];
	strncpy(sent_Command, sbuf, 4);
	if(strcmp(sent_Command, "SEND") == 0){
		check_command = 1;
		send (sd, sbuf, BUFLEN, 0);
		char fileOpening[] = "rb";
		//int getport, 
		initializeSecondSocket(sbuf, fileOpening, 5, 4, 10);
	}
	if(check_command == 0){
		printf("Error, GET/SEND PORT (4 digits) file\n");
	} 
	
	
	close (sd);
	return (0);
}


int getPort(char *buf, size_t length, size_t size){
	char *tmp_buf = &buf[length];
	tmp_buf[size] = '\0';
	return atoi(tmp_buf);
}

char* getFilepath(char *buf, size_t length){
	char *filepath = &buf[length];
	size_t index = 0;
	for(size_t i = 0; i < strlen(filepath); ++i){
		if(isspace(filepath[i])){
			index = i;
			break;
		}
	}
	filepath[index] = '\0';
	return filepath;
}

void initializeSecondSocket(char *sbuf, char *fileOpening, size_t length, size_t size, size_t fileLength){
	int GET_PORT = getPort(sbuf, length, size);
	printf("port:%d\n", GET_PORT);
	socklen_t second_client_len;
	struct sockaddr_in second_client;
	int second_sd;
		if ((second_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
			perror ("Can't create a socket");
			exit(1);
		}
		
		struct sockaddr_in secondServer;
		bzero((char *)&secondServer, sizeof(struct sockaddr_in));
		secondServer.sin_family = AF_INET;
		secondServer.sin_port = htons(GET_PORT);
		secondServer.sin_addr.s_addr = htonl(INADDR_ANY); 
		// Accept connections from any client

		if (bind(second_sd, (struct sockaddr *)&secondServer, sizeof(secondServer)) == -1){
			perror("Can't bind name to socket");
			close(second_sd);
			exit(1);
		}
		
		if(listen(second_sd, 1) < 0){
			perror("listen");
			exit(1);
		}
		while(TRUE){
			second_client_len= sizeof(second_client);
			int new_second_sd;
			if ((new_second_sd = accept (second_sd, (struct sockaddr *)&second_client, &second_client_len)) == -1){
				perror("accept");
				exit(1);
			}
			printf("Remote Address:  %s\n", inet_ntoa(second_client.sin_addr));
			char *filepath = getFilepath(sbuf, fileLength);
			FILE *file;
			if((file = fopen(filepath, fileOpening)) == NULL){
				fprintf(stderr, "Value of errno: %d\n", errno);
				perror("Error printed by perror");
				fprintf(stderr, "Error opening file: %s\n", strerror(errno));
			}
			//grab the file size
			unsigned char clientBuffer[BUFLEN];			
			size_t bytesToRead = 0;
			memset(clientBuffer, 0, BUFLEN);
			if(strncmp(fileOpening, "rb", 2) == 0){
				sendFile(clientBuffer, bytesToRead, file, new_second_sd);
			}
			if(strncmp(fileOpening, "wb", 2) == 0){
				writeFile(clientBuffer, bytesToRead, file, new_second_sd);
			}
			
			fclose(file);
			close(new_second_sd);
			break;
		}
		close(second_sd);
}

void writeFile(unsigned char *buffer, size_t bytesToRead, FILE *file, int socket){
	while((bytesToRead = recv(socket, buffer, BUFLEN, 0)) > 0){
		fwrite(buffer, 1, bytesToRead, file);
		printf("writing: %s\n", buffer);
		memset(buffer, 0, BUFLEN);
	}
}

void sendFile(unsigned char *buffer, size_t bytesToRead, FILE *file, int socket){	
	memset(buffer, 0, BUFLEN);
	while((bytesToRead = fread(buffer, 1, BUFLEN, file)) > 0){
		send(socket, buffer, bytesToRead, 0);
		printf("%s\n", buffer);
		memset(buffer, 0, BUFLEN);
	}
}