/*
 * server.c
 *
 *  Created on: Feb 25, 2018
 *      Author: karthika
 */


#include<stdio.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>

#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdarg.h>

#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<ctype.h>
#include<math.h>
#include<string.h>


// defining INADDR_NONE if its not already defined
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

extern int errno;

#define QUELEN 10
#define TRANSPORT "tcp"

/*
 * Handler function that executes Wait3 function whenever a child process exits
 */
void reaper(int);

/*
 * Creates a passive TCP socket for listening connections
 * Arguments:
 * portNumber: port where server will be listening for requests
 * transport: protocol used in the transport layer
 * queueLength: No of connections that can be kept in queue
 */
int passiveTCPsock(char* portNumber, char*transport);

/*
 * Print the error message and exits from the program
 * Arguments:
 * format: string constant given by user explaining the error condition
 * Can have variable number of inputs
 */
int errexit(const char* format, ...);


/*
 * Function for sending the file requested by client
 * Arguments: ssock (Connected socket for the client)
 */
int sendFile(int ssock);

int main(int argc, char* argv[]){

	char* host = "localhost";
	char* portNumber = "80";
	int msock, ssock, rd, w;
	unsigned int len;
	struct sockaddr_in fsin;
	char buf[1024];

	//Validations if hostname and portnumber are inserted as command line arguments
	if(argc == 2){
		host = argv[1];
	}
	else if(argc == 1){
		printf("No host and port number entered, assuming default values \n");
	}

	msock = passiveTCPsock(portNumber, TRANSPORT);
	printf("passive msock %d \n", msock);
	(void) signal(SIGCHLD, reaper);

	while(1){
		len = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr*)&fsin, &len);
		if(ssock < 0){
			errexit("accepting connection failed %s \n", strerror(errno));
		}
		else{
			printf("%d /n",ssock);
			printf("sucess \n");
			fflush(stdout);
		}
		switch(fork()){
		case 0: //child process
				(void)close(msock);
				 exit(sendFile(ssock));
				 break;
		case -1: errexit("forking error %s /n", strerror(errno));
				 break;
		default: //parent process
				 (void)close(ssock);
				 break;

		}
	}

}


int passiveTCPsock(char* portNumber, char*transport){

	//Creating passive socket
		struct protoent *ppe;
		struct sockaddr_in sin;
		int type, msock, b;


	//Creating the passive socket
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		uint16_t port = (uint16_t)atoi(portNumber);
		printf("port is %d \n",port);
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = INADDR_ANY;

	//Selecting type
		type = SOCK_STREAM;

	//Mapping protocol name to protocol number
		ppe = getprotobyname("tcp");
		if(ppe){
			fprintf(stdout, "protocol provided by the internet family \n");
		}
		else{
			errexit("transport protocol not provided by IP %s \n ", strerror(errno));
		}
	//Allocating socket
		msock = socket(PF_INET,type,ppe->p_proto);
		if(msock < 0){
			fprintf(stdout,"type %d \n",type);
			errexit("PAssive Socket not created properly %s \n", strerror((errno)));
		}
		fprintf(stdout, "msock is %d \n", msock);

	//Bind the socket
		b = bind(msock,(struct sockaddr*)&sin, sizeof(sin));
		if(b < 0){
			errexit("Not able to bind to port %s due to %s /n", portNumber, strerror(errno));
		}
		else{
			fprintf(stdout, "socket binded sucessfully to port %s \n", portNumber);
		}

	//Listening for connection
		if(listen(msock,QUELEN) <0){
			errexit("socket not listening for connection %s", strerror(errno));
		}
		return msock;
}

int errexit(const char *format, ...){

	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}

void reaper(int sig){
	int status;
	while(wait3(&status, WNOHANG, (struct rusage*)0) > 0){

	}
}

int sendFile(int ssock)
{
	int rd, w;
	int read_counter = 0;
	char buf[2048];
	char *request[3];
	char* filename = malloc(64);
	char buffer_file[2028];
	FILE *fptr;
	char* response;
	char* header = "Host:FileServer\r\nAccept-Ranges:bytes\r\nAllow:GET\r\nContent-Language:en\r\nContent-Type:application/octet-stream\r\nContent-Disposition:attachment\r\n\r\n";
	char* message;
	char buffer_server[4096];

	rd = read(ssock, buf, 2048);
	if(rd > 0)
	{
		printf("read success \n");
		printf(buf);
		buf[sizeof(buf)] = '\0';
	}
	else if(rd < 0)
	{
		printf("read unsuccessful \n");
		return -1;
	}

	//Splitting the input request
	printf("Splitting \n");
	request[0] = strtok(buf," ");
	request[1] = strtok(NULL, " " );
	request[2] = strtok(NULL, "\r\n\r\n");

	/*printf("printing the split up string and size \n");
	printf("%s %d \n",request[0], strlen(request[0]));
	printf("%s %d \n", request[1], strlen(request[1]));
	printf("%s %d \n", request[2], strlen(request[2]));*/

	if(strcmp(request[0], "GET") == 0)
	{
		printf("GET Method comparison successful \n");
	}
	else
	{
		printf("Only GET methods are supported in server \n");
		return -1;
	}
	if(strcmp(request[2],"HTTP/1.1") == 0 || strcmp(request[3], "HTTP/1.0") == 0)
	{
		printf("HTTP protocol version is successful \n");
	}
	else
	{
		printf("Only HTTP 1.0 and HTTP 1.1 versions are supported. \n");
		return -1;
	}

	filename = request[1];
	printf("filename is %s\n", filename);

	if(strcmp(filename,"/favicon.ico") == 0)
	{
		response = "HTTP/1.1 400 Bad-Request \r\n";
		printf(response);
		message = "Flavicon img not found";
		strcat(buffer_server, response);
		strcat(buffer_server, header);
		strcat(buffer_server, message);
		printf("server response is %s", buffer_server);
		w = write(ssock, buffer_server, strlen(buffer_server));
		close(ssock);
	}
	else if (strcmp(filename,"/") == 0)
	{
		printf("No file name specified\n");
		response = "HTTP/1.1 400 Bad-Request\r\n";
		message = "Provide File name";
		strcat(buffer_server, response);
		strcat(buffer_server, header);
		strcat(buffer_server, message);
		printf("server response is %s", buffer_server);
		w = write(ssock, buffer_server, strlen(buffer_server));
		close(ssock);

	}
	else if(filename[0] == '/' && strlen(filename) > 2)
	{
		printf("file name provided by client %s\n", filename);
		printf(filename);

		if(access(filename, F_OK) == -1)
		{
			printf("Error in opening file access \n");
			//Sending HTTP response
			response = "HTTP/1.1 404 Not-Found\r\n";
			message = "File Not Found";
			strcat(buffer_server, response);
			strcat(buffer_server, header);
			strcat(buffer_server, message);
			printf("server response is %s", buffer_server);
			w = write(ssock, buffer_server, strlen(buffer_server));
			close(ssock);
			return 0;
		}
		//Sending file
		fptr = fopen(filename,"r");

		if(fptr < 0)
		{
			printf("Error in opening file \n");
			return -1;
		}
		printf("reading file\n");
		while(!feof(fptr))
		{
			rd = fread(&buffer_file[read_counter], 1, 256, fptr);
			if(rd < 0)
			{
				printf("Error in reading \n");
				return -1;
			}
			read_counter+=rd;
		}
		printf("file size is %d\n", read_counter);
		//Sending HTTP response
		response = "HTTP/1.1 200 OK\r\n";
		strcat(buffer_server, response);
		strcat(buffer_server, header);
		strcat(buffer_server, buffer_file);
		printf("server response is %s", buffer_server);
		w = write(ssock, buffer_server, strlen(buffer_server));

	}
	else if(strlen(filename) > 2 && filename[0]!= '/')
	{
		printf("file name provided by client without path %s\n", filename);

				if(access(filename, F_OK) == -1)
				{
					printf("Error in opening file without / \n");
					//Sending HTTP response
					response = "HTTP/1.1 404 Not-Found\r\n";
					message = "File Not Found";
					strcat(buffer_server, response);
					strcat(buffer_server, header);
					strcat(buffer_server, message);
					printf("server response is %s", buffer_server);
					w = write(ssock, buffer_server, strlen(buffer_server));
					close(ssock);
					return 0;
				}
				//Sending file
				fptr = fopen(filename,"r");

				if(fptr < 0)
				{
					printf("Error in opening file \n");
					return -1;
				}
				printf("reading file\n");
				while(!feof(fptr))
				{
					rd = fread(&buffer_file[read_counter], 1, 256, fptr);
					if(rd < 0)
					{
						printf("Error in reading \n");
						return -1;
					}
					read_counter+=rd;
				}
				printf("file size is %d\n", read_counter);
				//Sending HTTP response
				response = "HTTP/1.1 200 OK\r\n";
				strcat(buffer_server, response);
				strcat(buffer_server, header);
				strcat(buffer_server, buffer_file);
				printf("server response is %s", buffer_server);
				w = write(ssock, buffer_server, strlen(buffer_server));
	}

	fclose(fptr);
	close(ssock);

}



