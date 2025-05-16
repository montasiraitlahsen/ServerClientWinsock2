#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
typedef struct
{
    char username[100];
    char buffer[100];
}Client;
SOCKET ClientSocket;
Client param;
int ConnectClient=0;
unsigned __stdcall receivingClient(void *param);
int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }
    struct sockaddr_in Server,ClientData;
    ClientSocket=socket(AF_INET,SOCK_STREAM,0);
    if(ClientSocket==INVALID_SOCKET)
    {
        printf("broken socket\n");
        closesocket(ClientSocket);
        return 1;
    }
    Server.sin_family=AF_INET;
    Server.sin_port=htons(2000);
    Server.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    int iResult=connect(ClientSocket,(const struct sockaddr *)&Server,sizeof(Server));
    char Username[100];
    if(iResult==0 && ConnectClient==0)
    {
        printf("enter your name :");
        fgets(Username,sizeof(Username),stdin);
        send(ClientSocket,Username,strlen(Username),0);
        ConnectClient++;
    }
    HANDLE ThreadReceive = (HANDLE)_beginthreadex(NULL ,0,receivingClient,&param,0,NULL);
    if(ThreadReceive)
    {
        CloseHandle(ThreadReceive);
    }
    char Buffer[100];
    while(TRUE)
    {
        printf("send a message :");
        fgets(Buffer,sizeof(Buffer),stdin);
        if(strncmp(Buffer,"quit",4)==0)
        {
            break;
        }
        int test=send(ClientSocket,Buffer,strlen(Buffer),0);
    }
    DWORD waitResult = WaitForSingleObject(ThreadReceive,3000);
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
unsigned __stdcall receivingClient(void *param)
{
    Client Clients = *(Client *)param;
    int Result;
    while(TRUE)
    {
        Result = recv(ClientSocket,Clients.username,sizeof(Clients.username),0);
        if(Result <= 0)
        {
            break;
        }
        Result = recv(ClientSocket,Clients.buffer,sizeof(Clients.buffer),0);
        if(Result <= 0)
        {
            break;
        }
        printf("\n%s says : %s",Clients.username,Clients.buffer);
        printf("send a message :");
    }
    return 0;
}
