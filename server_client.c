#pragma comment(lib,"ws2_32.lib")
#include <stdlib.h>
#include<WinSock2.h>
#include<stdio.h>
#include<string.h>
#include <windows.h>
#include <tchar.h>
void ClientHandlerTread()
{
	SOCKADDR_IN addr; //Address that we will bind our listening socket to 

	int addrlen = sizeof(addr); //length of the address (required for accept call)
	addr.sin_addr.s_addr = INADDR_ANY; //Broadcast locally
	addr.sin_port = htons(3333); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET sListen = socket(AF_INET, SOCK_DGRAM, NULL); //Create socket to listen for new connections
	if (bind(sListen, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("Bind error.\n");	
	}
	 char otherName[256];
	 memset(otherName, 0, 256);
	  recv(sListen, otherName, sizeof(otherName), NULL); //recieve message to buffer  
	  char buffer[256];   

while(true)
	{
		memset(buffer, 0, 256);
		recv(sListen, buffer, sizeof(buffer), NULL); //recieve message to buffer	
		printf("%s ->   %s \n", otherName, buffer);

	}
}

int main(int argc , char *argv[])
{	int i=0;
	for(i=1;i<argc;i++)
	{
		printf("argv[%d] = %s \n", i, argv[i]);
	}

	int PORT=atoi(argv[2]);
	char name[30];
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	SOCKET Connection;
    printf("Your Name-> ");
	gets(name);
	printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //Create a socket
    if((Connection = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");




	SOCKADDR_IN addr; //Address to be binded our Connection socket
	int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function
	addr.sin_addr.s_addr = inet_addr(argv[1]); //Address = localhost (this pc)
	addr.sin_port =htons(PORT); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket
	int addrlen = sizeof(addr);
	
	sendto(Connection, name, strlen(name), 0, (struct sockaddr *)&addr, addrlen);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerTread, NULL, NULL, NULL); // Create Thread to this Client.
	char buffer[256];
	while (true)
	{
		memset(buffer, 0, 256);
		gets(buffer);
		//if (send(Connection, buffer, strlen(buffer), NULL) == SOCKET_ERROR)
		if (sendto(Connection, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, addrlen) == SOCKET_ERROR)
		{
			printf("Send error\n");
		}
	}
	return 0;

}
