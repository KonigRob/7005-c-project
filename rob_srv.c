/*-----------------------------------------------------------------------------
 * SOURCE FILE:    rob_srv.c
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
 * The program will accept a TCP connection client machines using port 7005.
 * The server will respond to the connected client by sending a requested file,
 * or transferring a file onto the server. 
* --------------------------------------------------------------------------*/
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define SERVER_TCP_PORT 7005
#define BUFLEN	80		//Buffer length
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
void initializeSecondSocket(char *buf, struct hostent *hp, struct sockaddr_in client, const char *fileFormat, size_t length, size_t size, size_t fileLength);

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
 * Establishes a connection with any client that is connecting on port 7005.
 * Then listens for a sent message wheather it is SENT or GET.
 * -----------------------------------------------------------------------*/
int main (int argc, char **argv){
	int	n, bytes_to_read, sd, new_sd, port;
	socklen_t client_len;
	struct	sockaddr_in server, client;
	struct hostent	*hp;
	char *bp, buf[BUFLEN];

	switch(argc){
		case 1:
			port = SERVER_TCP_PORT;	// Use the default port
		break;
		case 2:
			port = atoi(argv[1]);	// Get user specified port
		break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}

	// Create a stream socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror ("Can't create a socket");
		exit(1);
	}

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); 
	
	// Accept connections from any client
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		perror("Can't bind name to socket");
		close(sd);
		exit(1);
	}

	// Listen for connections

	// queue up to 5 connect requests
	listen(sd, 5);

	while (TRUE){
		client_len= sizeof(client);
		if ((new_sd = accept (sd, (struct sockaddr *)&client, &client_len)) == -1){
			fprintf(stderr, "Can't accept client\n");
			exit(1);
		}

		printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
		bp = buf;
		bytes_to_read = BUFLEN;
		while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFLEN){
			bp += n;
			bytes_to_read -= n;
		}
		
        //grabbing the first 3 chars of the instructions
        char get_Check[4];
        strncpy(get_Check, buf, 3);
        if(strcmp(get_Check, "GET") == 0){
			char fileOpening[] = "rb";
			initializeSecondSocket(buf, hp, client, fileOpening, 4, 4, 9);
        }
        char send_Check[5];
        strncpy(send_Check, buf, 4);
        if(strcmp(send_Check, "SEND") == 0){
			char fileOpening[] = "wb";
			initializeSecondSocket(buf, hp, client, fileOpening, 5, 4, 10);
        }
		send (new_sd, buf, BUFLEN, 0);
		close (new_sd);
	}
	close(sd);
	return(0);
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

void initializeSecondSocket(char *buf, struct hostent *hp, struct sockaddr_in client, const char *fileFormat, size_t length, size_t size, size_t fileLength){
	int SEND_PORT = getPort(buf, length, size);
			
	int second_sd;
	if ((second_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Cannot create another socket");
		exit(1);
	}
	struct	sockaddr_in second_server;
	bzero((char *)&second_server, sizeof(struct sockaddr_in));
	second_server.sin_family = AF_INET;
	second_server.sin_port = htons(SEND_PORT);
	if ((hp = gethostbyname(inet_ntoa(client.sin_addr))) == NULL){
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hp->h_addr, (char *)&second_server.sin_addr, hp->h_length);
	// Connecting to the server (which is the client)
	if (connect (second_sd, (struct sockaddr *)&second_server, sizeof(second_server)) == -1){
		fprintf(stderr, "Can't connect to server\n");
		perror("connect");
		exit(1);
	}
	printf("Connected:    Server Name: %s:%d\n", hp->h_name,SEND_PORT);

	char *filepath = getFilepath(buf, fileLength);
			
	FILE *file;
	if((file = fopen(filepath, fileFormat)) == NULL){
		fprintf(stderr, "Value of errno: %d\n", errno);
		perror("Error printed by perror");
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	}

	unsigned char serverBuffer[BUFLEN];
	int bytesToRead = 0;
	if(strncmp(fileFormat, "wb", 2) == 0){
		writeFile(serverBuffer, bytesToRead, file, second_sd);
	}
	if(strncmp(fileFormat, "rb", 2) == 0){
		sendFile(serverBuffer, bytesToRead, file, second_sd);
	}
	
	fclose(file);
	printf("File transferred!: %s\n", filepath);

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