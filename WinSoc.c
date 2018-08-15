#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


void WSAAPI freeaddrinfo( struct addrinfo* );

int WSAAPI getaddrinfo( const char*, const char*, const struct addrinfo*,
                 struct addrinfo** );

int WSAAPI getnameinfo( const struct sockaddr*, socklen_t, char*, DWORD,
                char*, DWORD, int );

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define LINELENGTH 65535
#define DEFAULT_PORT "80"



int __cdecl main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = "/";
   // char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    //int recvbuflen = DEFAULT_BUFLEN;
    char* request;
    char* request_post;
    char* request_post1;
    char *filepath;
    char buf[LINELENGTH+1];
    char buf_read[LINELENGTH+1];
    char buf_read1[LINELENGTH+1];
    char* server = "localhost";
    int w,reading;


    char* header = "Host: 192.168.1.5\r\nUser-Agent: Chrome/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
               "Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nContent-Type: application/x-www-form-urlencoded\r\n"
               "Content-Length: 23\r\n\r\n";

       char* header1 = "Host: 192.168.1.5\r\nUser-Agent: Chrome/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
                   "Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\n\r\n";

       char* header2 = "Host: 127.0.0.1\r\nUser-Agent: Chrome/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
                   "Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nContent-Type: application/x-www-form-urlencoded\r\n"
                   "\r\n";

       //Memory allocation for pointers
        filepath = malloc(256);
        request = malloc(65536);
        request_post = malloc(65536);
        request_post1 = malloc(65536);

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

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            printf("Socket error \n");
            continue;
        }
        break;
    }

   // freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    printf("connection sucessful \n");
    //Creating an http request
    FILE *fptr;
    fptr = fopen("loginfile", "w");
    memset(request_post,0, 65536);
    strcat(request_post,"GET ");
    strcat(request_post, "/ HTTP/1.1\r\n");
    strcat(request_post, header1);

    printf(request_post);
    w = send(ConnectSocket,request_post,(int)strlen(request_post),0);

    if (w == SOCKET_ERROR) {
    	printf("send failed with error: %d\n", WSAGetLastError());
    	closesocket(ConnectSocket);
    	WSACleanup();
    	return 1;
    }
    printf("Bytes Sent: %ld\n", w);


   /* while((reading = recv(ConnectSocket, buf, LINELENGTH+1,0))>0)
    {
    	printf("read bytes %d\n",reading);
    	fwrite(buf,1,reading,fptr);
    	memset(buf,0, 65536);
    }*/

    reading = recv(ConnectSocket, buf, LINELENGTH+1,0);
    fwrite(buf,1,reading,fptr);
    memset(buf,0, 65536);
    printf(buf);
    printf("\n");
    fclose(fptr);

    memset(buf, 0, sizeof(buf));
    reading = 0;

    FILE *fptr1;
    fptr1 = fopen("existing user", "w");
    memset(request_post1,0, 65536);
    strcat(request_post1,"POST ");
    strcat(request_post1, "/existing_user HTTP/1.1\r\n");
    strcat(request_post1, header2);
    //printf(request_post1);
    //printf("len = %d\n", strlen(request_post1));

    w = send(ConnectSocket,request_post1,(int)strlen(request_post1),0);
    /*while((reading = recv(ConnectSocket, buf, LINELENGTH+1,0))>0)
    {
    	printf("REad size = %d \n", reading);
    	fwrite(buf,1,reading,fptr1);
    	memset(buf,0,LINELENGTH+1);
    }*/

    reading = recv(ConnectSocket, buf, LINELENGTH+1,0);
    fwrite(buf,1,reading,fptr1);
    memset(buf,0,LINELENGTH+1);

    fclose(fptr1);

    memset(buf,0, LINELENGTH+1);
    reading = 0;
    FILE *fptr2;
    fptr2 = fopen("patient.html", "w");

        //Writing to server
    memset(request,0, 65536);
    strcat(request,"POST ");
    strcat(request, "/result HTTP/1.1\r\n");

    strcat(request, header);

    strcat(request,"uname=p0123&psw=prakash");

    printf("\n");
    printf(request);
    int counter = 0;

    w = send(ConnectSocket,request,(int)strlen(request),0);


    if(w<0){
    	printf("write not happening \n");
    }
    else{
    	printf("%d bytes written by client\n", w);
    }


  //Reading response from the server
   /* while((reading = recv(ConnectSocket, buf, LINELENGTH+1,0))>0)
       {
       	printf(buf);
       	fwrite(buf,1,reading,fptr2);
       	memset(buf,0,LINELENGTH+1);
       }*/

    reading = recv(ConnectSocket, buf, LINELENGTH+1,0);
    printf(buf);
    printf("bytes read %d \n", reading );
    fwrite(buf,1,reading,fptr2);
    fclose(fptr2);

    // Send an initial buffer
   /* iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }*/



    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // Receive until the peer closes the connection
    /*   do {

           iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
           if ( iResult > 0 )
           {
           	 printf("Bytes received: %d\n", iResult);
           	       	   printf(recvbuf);
           }

           else if ( iResult == 0 )
               printf("Connection closed\n");
           else
               printf("recv failed with error: %d\n", WSAGetLastError());

       } while( iResult > 0 );*/

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
