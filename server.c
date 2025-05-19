#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <string.h>
#include <stdbool.h>
#pragma comment(lib, "ws2_32.lib")
// for th user is message
char Buffer[100];
typedef struct 
{
    char Username[100];
    SOCKET Clients;
}Clients;
int UsernameCount;
typedef struct
{
    Clients User[100];
    char Recipient[100];
    bool GeneralPrivate;
}Into;
Into Message;
int Counter=0;
// for receiving and printing message from teh client
unsigned __stdcall ReceivingAndPrintingData(void *param);
int main()
{
    Message.GeneralPrivate = FALSE;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }
    struct sockaddr_in Server,ClientData;
    SOCKET ServerSocket=socket(AF_INET,SOCK_STREAM,0);
    if(ServerSocket==INVALID_SOCKET)
    {
        printf("broken socket\n");
        closesocket(ServerSocket);
        return 1;
    }
    Server.sin_family=AF_INET;
    Server.sin_port=htons(2000);
    Server.sin_addr.S_un.S_addr=INADDR_ANY;
    if(bind(ServerSocket, (struct sockaddr *)&Server, sizeof(Server)) == SOCKET_ERROR)
    {
    printf("Bind failed with error: %d\n", WSAGetLastError());
    closesocket(ServerSocket);
    WSACleanup();
    return 1;
    }
    if(listen(ServerSocket,5)==SOCKET_ERROR)
    {
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }
    printf("the server is listening\n");
    int sizeData=sizeof(ClientData);
    SOCKET Client;
    while(TRUE)
    {
        Client=accept(ServerSocket,(struct sockaddr *)&ClientData,&sizeData);
        SOCKET *ClientAdress=(SOCKET *)malloc(sizeof(SOCKET));
        *ClientAdress = Client;
        HANDLE ClientThread = (HANDLE)_beginthreadex(NULL,0,ReceivingAndPrintingData,ClientAdress,0,NULL);
        if(ClientThread)
        {
            CloseHandle(ClientThread);
        }
    }
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}
unsigned __stdcall ReceivingAndPrintingData(void *param)
{
    char Username[100];
    bool Check=FALSE;
    memset(Username,0,sizeof(Username));
    int resultnumber = recv(*(SOCKET*)param,Username,sizeof(Username),0);
    int len = strlen(Username);
    while (len > 0 && (Username[len-1] == '\n' || Username[len-1] == '\r' || 
    Username[len-1] == '\t' || Username[len-1] == ' '))
    {
        Username[--len] = '\0';
    }
    if(resultnumber > 0)
    {
        for(int i=0;i<Counter;i++)
        {
            if(strcmp(Message.User[i].Username,Username)==0)
            {
                Check=TRUE;
            }
        }
        if(!Check)
        {
            Message.User[Counter].Clients=*(SOCKET *)param;
            strcpy(Message.User[Counter++].Username,Username);
            UsernameCount++;
        }
    }
    Message.GeneralPrivate = TRUE;
    while(TRUE)
    {  
        memset(Message.Recipient,0,sizeof(Message.Recipient));
        int resultnumberb = recv(*(SOCKET*)param,Message.Recipient,sizeof(Message.Recipient),0);
        int lenr = strlen(Message.Recipient);
        while (lenr > 0 && (Message.Recipient[lenr-1] == '\n' || Message.Recipient[lenr-1] == '\r' || 
        Message.Recipient[lenr-1] == '\t' || Message.Recipient[lenr-1] == ' '))
        {
            Message.Recipient[--lenr] = '\0';
        }
        if(strcmp(Message.Recipient," ") == 0)
        {
            Message.GeneralPrivate = FALSE;
        }
        memset(Buffer,0,sizeof(Buffer));
        int resultnumber = recv(*(SOCKET*)param,Buffer,sizeof(Buffer),0);
        int len = strlen(Buffer);
        while (len > 0 && (Buffer[len-1] == '\n' || Buffer[len-1] == '\r' || 
        Buffer[len-1] == '\t' || Buffer[len-1] == ' '))
        {
            Buffer[--len] = '\0';
        }
        if(resultnumber>0)
        {
            if(!Message.GeneralPrivate || (strcmp(Message.Recipient,"all") == 0))
            {
                for(int i=0;i<Counter;i++)
                {
                    if(Message.User[i].Clients==*(SOCKET *)param)
                    {
                        for(int i=0;i<Counter;i++)
                        {
                            if(Message.User[i].Clients!=*(SOCKET *)param)
                            {
                                send(Message.User[i].Clients,Username,strlen(Username),0);
                                send(Message.User[i].Clients,Buffer,strlen(Buffer),0);
                            }
                        }
                    }
                }
            }
            else
            {
                int  searching = 0;
                for(int i=0;i<Counter;i++)
                {
                    if(strcmp(Message.Recipient,Message.User[i].Username) == 0)
                    {
                        send(Message.User[i].Clients,Username,strlen(Username),0);
                        send(Message.User[i].Clients,Buffer,strlen(Buffer),0);
                    }
                    else
                    {
                        searching++;                        
                    }
                }
                if(searching == Counter)
                {
                    printf("the recipient are not found \n");
                }
            }
        }
    }
    closesocket(*(SOCKET*)param);
    free(param);
    return 0;
}