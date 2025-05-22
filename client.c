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
char Recipient[100];
unsigned __stdcall receivingClient(void *param);
unsigned __stdcall ReceiveRecipient(void *param);
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
    char test[10];
    HANDLE ThreadReceive = (HANDLE)_beginthreadex(NULL ,0,receivingClient,&param,0,NULL);
    if(ThreadReceive)
    {
        CloseHandle(ThreadReceive);
    }
    char Buffer[100];
    while(TRUE)
    {
        printf("what is the name of the recipient :");
        fgets(Recipient,sizeof(Recipient),stdin);
        int lenr = strlen(Recipient);
        while (lenr > 0 && (Recipient[lenr-1] == '\n' || Recipient[lenr-1] == '\r' || 
        Recipient[lenr-1] == '\t' || Recipient[lenr-1] == ' '))
        {
            Recipient[--lenr] = '\0';
        }
        int testa=send(ClientSocket,Recipient,strlen(Recipient),0);
        printf("send a message :");
        fgets(Buffer,sizeof(Buffer),stdin);
        int lenb = strlen(Buffer);
        while (lenb > 0 && (Buffer[lenb-1] == '\n' || Buffer[lenb-1] == '\r' || 
        Buffer[lenb-1] == '\t' || Buffer[lenb-1] == ' '))
        {
            Buffer[--lenb] = '\0';
        }
        if(strcmp(Buffer,"quit")==0)
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
        memset(Clients.username,0,sizeof(Clients.username));
        Result = recv(ClientSocket,Clients.username,sizeof(Clients.username),0);
        if(Result <= 0)
        {
            break;
        }
        int lena = strlen(Clients.username);
        while(lena > 0 && (Clients.username[lena-1] == '\n' || Clients.username[lena-1] == '\r' || 
        Clients.username[lena-1] == '\t' || Clients.username[lena-1] == ' '))
        {
            Clients.username[--lena] = '\0';
        } 
        memset(Clients.buffer,0,sizeof(Clients.buffer));
        Result = recv(ClientSocket,Clients.buffer,sizeof(Clients.buffer),0);
        if(Result <= 0)
        {
            break;
        }
        int len = strlen(Clients.buffer);
        while (len > 0 && (Clients.buffer[len-1] == '\n' || Clients.buffer[len-1] == '\r' || 
                          Clients.buffer[len-1] == '\t' || Clients.buffer[len-1] == ' '))
        {
            Clients.buffer[--len] = '\0';
        }
        printf("\n-->%s says : %s",Clients.username,Clients.buffer);
        printf("\nsend a message : ");
    }
    return 0;
}