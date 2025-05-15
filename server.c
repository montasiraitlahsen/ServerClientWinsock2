#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
// for th user is message
char Buffer[100];
typedef struct 
{
    char Username[100];
    SOCKET Clients;
}Clients;
int UsernameCount;
Clients User[100];
int Counter=0;
// for receiving and printing message from teh client
unsigned __stdcall ReceivingAndPrintingData(void *param);
int main()
{
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
        UsernameCount=0;
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
    while(TRUE)
    {
        memset(Buffer, 0, sizeof(Buffer));
        int resultnumber = recv(*(SOCKET*)param,Buffer,sizeof(Buffer),0);
        if(resultnumber>0 && UsernameCount==0)
        {
            Buffer[resultnumber]='\0';
            User[Counter].Clients=*(SOCKET *)param;
            strcpy(User[Counter++].Username,Buffer);
            UsernameCount++;
        }
        else if(resultnumber>0)
        {
            Buffer[resultnumber]='\0';
            if(strncmp(Buffer,"quit",4)==0)
            {
                break;
            }
            else
            {
                for(int i=0;i<Counter;i++)
                {
                    if(User[i].Clients==*(SOCKET *)param)
                    {
                        printf(" %s says : %s",User[i].Username,Buffer);
                    }
                }
            }

        }
        else
        {
            break;
        }
    }
    closesocket(*(SOCKET*)param);
    free(param);
    return 0;
}