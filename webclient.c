/*
 * webclient.c
 *
 *  Created on: Apr 4, 2018
 *      Author: karthika
 */


/*
 * client.c
 *
 *  Created on: Feb 24, 2018
 *      Author: karthika
 */


#include<stdio.h>

#include<sys/socket.h>
#include<sys/types.h>

#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdarg.h>

#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<ctype.h>

// defining INADDR_NONE if its not already defined
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

extern int errno;

/*
 * This function is defined to connect the client to the server
 * Arguments
 * server: server to which the host is to be connected
 * service: the name of service client needs
 * transport: the transport protocol through which client is sending requests
 */
int connectTCP (char *server, char *transport, char *portnumber);

/*
 * Used to print error message and exits. Funtion can accept variable number of arguments
 * Argument:
 * format: string type variable describing user given error description
 *
 */
int errexit(const char *format, ...);

/*
 * Checks whether the string passed as parameter is a number or not
 * Also validates whether its positive or negative
 * if negative the function exits as square root of negative number cannot be found
 */


#define LINELENGTH 65535

int main(int argc, char *argv[])
{

	char* server = "localhost";
	char* transport = "tcp";
	char *portnumber = "80";
	int sock,n,w;
	char* request;
	char* request_post;
	char* request_post1;
	char *filepath;
	char buf[LINELENGTH+1];
	char buf_read[LINELENGTH+1];
	char buf_read1[LINELENGTH+1];
	char*file;
	int reading;
	int read_total = 0;
	int size = LINELENGTH+1;
	//char* header = "Host:Webclient\r\nContent-Language:en\r\nContent-Type:application/x-www-form-url-encoded\r\nContent-Length:15\r\n\r\n";
	char* header = "Host: 127.0.0.1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
			"Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nContent-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: 23\r\n\r\n";

	char* header1 = "Host: 127.0.0.1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
				"Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\n\r\n";

	char* header2 = "Host: 127.0.0.1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
				"Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nContent-Type: application/x-www-form-urlencoded\r\n"
				"\r\n";


	//Memory allocation for pointers
	filepath = malloc(256);
	request = malloc(65536);
	request_post = malloc(65536);
	request_post1 = malloc(65536);

	//Validation to check if the inputs are properly entered
	if(argc == 3){
		filepath = argv[1];
		server = argv[2];
	}
	else if (argc == 2){
		printf("File not entered. Connecting to server without any file request\n");
		filepath = "nofile";
		server = argv[1];
	}
	else
	{
		printf("please enter file and server details \n");
		exit(1);
	}

	//Connecting to server
	sock = connectTCP(server, transport, portnumber);


	/*strcat(request_post, "GET / HTTP/1.1\r\n");
	strcat(request_post, header1);
	strcat(request_post, "\r\n");*/
	FILE *fptr;
	fptr = fopen("loginfile", "w");

	strcat(request_post,"GET ");
	strcat(request_post, "/ HTTP/1.1\r\n");
	strcat(request_post, header1);

	printf(request_post);

	//printf("len = %d", strlen(request_post));
	w = write(sock,request_post,strlen(request_post));
	printf("write done and reading happening\n");
	reading = read(sock, buf, LINELENGTH+1);
	printf("read");

	fwrite(buf,1,reading,fptr);
	fclose(fptr);

	memset(buf, 0, sizeof(buf));
	reading = 0;

	FILE *fptr1;
	fptr1 = fopen("existing user", "w");

	strcat(request_post1,"POST ");
	strcat(request_post1, "/existing_user HTTP/1.1\r\n");
	strcat(request_post1, header2);

	printf(request_post1);
	printf("len = %d\n", strlen(request_post1));
	w = write(sock,request_post1,strlen(request_post1));
	reading = read(sock, buf, LINELENGTH+1);
	fwrite(buf,1,reading,fptr1);
	fclose(fptr1);

	memset(buf,0, LINELENGTH+1);
	reading = 0;
	FILE *fptr2;
	fptr2 = fopen("patient.html", "w");

	//Writing to server
	strcat(request,"POST ");
	strcat(request, "/result HTTP/1.1\r\n");

	strcat(request, header);

	strcat(request,"uname=p0123&psw=prakash");

	printf("\n");
	printf(request);
	int counter = 0;

	w = write(sock,request,strlen(request));


	if(w<0){
		errexit("write not happening %s \n", strerror(errno));
	}
	else{
		printf("%d bytes written by client\n", w);
	}


	//Reading response from the server


	 reading = read(sock, buf, LINELENGTH+1);
	 printf(buf);
	 printf("bytes read %d \n", reading );
	 fwrite(buf,1,reading,fptr2);
	 fclose(fptr);
	 close(sock);
	return 0;
}

int connectTCP (char *server, char *transport, char *portnumber){

	int counter_num = 0;
	int counter_ip = 0;
	int counter_hostname = 0;
	char *ip;
	char *hostname;
	int sock,type;
	int optval;
	socklen_t optlen = sizeof(optval);

	struct sockaddr_in sin;
	struct servent *pse;
	struct hostent *phe;
	struct protoent *ppe;

//validating if portnumber is numeric
	for(int i = 0; i< strlen(portnumber); i++){
		if(!(isdigit(portnumber[i]))){
			errexit("error portnumber is not all numeric \n");
		}
	}
	fprintf(stdout, "port number entered is all numeric \n");

//validating if server name passed is all numeric
/*	for(int i = 0; i<strlen(server); i++){
		if(isdigit(server[i])){
			counter_num++;
		}
	}*/

/*	if(counter_num == strlen(server)){
		errexit("host name or host ip cannot be all numeric \n");
	}*/


///validating if the server input is ip
	/*for(int j = 0; j<strlen(server); j++){
		if((isdigit(server[j])) || (server[j] == '.')){
			printf("ip is %s \n", server[j]);
			counter_ip++;
		}
		else{
			break;
		}
	}*/

//validating if the server input is hostname
/*	for(int i = 0; i< strlen(server); i++){
		if((isalnum(server[i])) || ispunct(server[i])){
			counter_hostname++;
		}
		else{
			break;
		}
	}*/

	//assigning values
/*	if(counter_ip == strlen(server)){
		ip = server;
	}
	else if(counter_hostname == strlen(server)){
		hostname = server;
	}
	else{
		errexit("value input for hostname/ip is not in the correct format \n");
	}*/

//setting all values to 0 in sockaddr_in
	memset(&sin, 0 , sizeof(sin));
//setting family name and portnumber
	sin.sin_family = AF_INET;


	//converting portnumber to integer
	uint16_t port =(uint16_t) atoi(portnumber);
	sin.sin_port = htons(port);

//setting the type of packet
	if(transport == "tcp"){
		type = SOCK_STREAM;
	}
	else if(transport == "udp"){
		type = SOCK_DGRAM;
	}
	else{
		errexit("transport type couldnot be matched \n");
	}

//mapping hostname or ip addresses
		ip = server;

		if(inet_addr(ip) != INADDR_NONE){
			sin.sin_addr.s_addr = inet_addr(ip);
			fprintf(stdout, "ip address successfully mapped \n");
		}
		else{
			errexit("ip address given is not valid %s \n", strerror(errno));
		}

	/*else if(hostname){
		phe = gethostbyname(hostname);
		if(phe){
			memcpy(&sin.sin_addr.s_addr, phe->h_addr, phe->h_length);
			fprintf(stdout, "hostname successfully mapped to ip address \n");
		}
		else if((sin.sin_addr.s_addr = inet_addr(hostname)) == INADDR_NONE){
			errexit("host name is not correct %s \n", strerror(errno));
		}
	}*/

//Validating protocol number
	if((ppe = getprotobyname(transport)) == 0){
		errexit("Cant get the protocol entry for protocol %s \n" , transport);
	}

//Creating socket
	sock = socket(PF_INET, type, ppe ->p_proto);

	if(sock < 0){
		errexit("Socket not created properly %s \n", strerror(errno));
	}

	//Setting the keep alive option for the socket
	if(getsockopt(sock,SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0)
	{
		printf("error in getting the keep alive option \n");
		exit(1);
	}

	if(optval == 0)
	{
		printf("Keep alive option not set for socket \n");
		optval = 1;
		if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
		{
			printf("error in setting the keep alive option \n");
			exit(1);
		}
		else
		{
			printf("success setting keep alive option \n ");
		}
	}

	//Connecting to server
	if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) <0){
		errexit("connection to server failed %s \n",strerror(errno));
	}
	else{
		fprintf(stdout,"connected to server sucesssfully \n");
	}
	return sock;
}

int errexit(const char *format, ...){
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}


