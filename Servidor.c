#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>

#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>


#define PORT 4444
#define MAIN_PARAMETER_1 1

#include "libs.h"

void *handle_client(void *arg);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int i = 0, count =0, sockfd;


int main(int32_t argc, char *argv[])
{
    struct sockaddr_in serverAddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }

    printf("[+]Server Socket is created.\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", 4444);

    if(listen(sockfd, 5) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]Error conection.\n");
    }

    struct sockaddr_in newAddr;


    while(1)
    {
        ConnAck enviado;
        Connect recibido;
        
        enviado.FrameType = 0x20;
        enviado.MsgLen = 0x02;
        enviado.Flags = 0x00;
        enviado.ReturnStatus = 0x00;

        socklen_t addr_size = sizeof(newAddr);

        int newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
        count ++;
        if(newSocket < 0)
        {
            printf("[-]Accept failed.\n");
            count --;
            return -1;
        }

        memset(&recibido, 0, sizeof(recibido));

        int valread = recv(newSocket, &recibido, 1024, 0);
        int total = 0x0008 + strlen(recibido.User) + strlen(recibido.ClientId);

        if(valread <0)
        {
            printf("[-]The data was not received\n");
            printf("[-]Shutting down the socket %i\n", count);
            count --;
            close(newSocket);
        }


        else if(recibido.FrameType!=0x10 || recibido.Version != 0x04 || recibido.ConnectFlags !=0x20 || strlen(recibido.User) != recibido.UserLen || total != recibido.MsgLen)
        {
            printf("[-]Wrong data recieved\n");
            enviado.ReturnStatus = 0x0E;

            if(send(newSocket, &enviado, sizeof(enviado), 0) <= 0) 
            {
                printf("[-]The data was not sended\n");
                printf("[-]Shutting down the socket %i\n", count);
                close(newSocket);
            }

            else 
            {

                printf("\nSENDED FRAME\n");
                printf("Frame type: 0x%02X\n", enviado.FrameType);
                printf("Message Lenght: 0x%02X\n", enviado.MsgLen);
                printf("Flags: 0x%02X\n", enviado.Flags);
                printf("Return status: 0x%02X\n", enviado.ReturnStatus);

                printf("\nRECEIVED FRAME\n");
                printf("Frame type: 0x%02X\n", recibido.FrameType);
                printf("Message Lenght: 0x%02X\n", recibido.MsgLen);
                printf("User Len: 0x%02X\n", recibido.UserLen);
                printf("User: %s\n", recibido.User);
                printf("Version: 0x%02X\n", recibido.Version);
                printf("Connect Flags: 0x%02X\n", recibido.ConnectFlags);
                printf("Keep Alive: 0x%02X\n", recibido.KeepAlive);
                printf("Cliente id lenght: 0x%02X\n", recibido.ClientIdLen);
                printf("Client id: %s\n", recibido.ClientId);

                

            }

            printf("[-]Shutting down the socket %i\n", count);
            count --;
            close(newSocket);
        }
        
        else 
        {
            printf("[+]Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            
            if(send(newSocket, &enviado, sizeof(enviado), 0) <= 0) 
            {
                printf("[-]The data was not sended\n");
                printf("[-]Shutting down the socket %i\n", count);
                close(newSocket);
            }

            else 
            {
                pthread_t tid, tid2;

                if(pthread_create(&tid, NULL, handle_client,(void *)&newSocket) <0)
                {
                    printf("[-]Thread failed.\n");
                    printf("[-]Shutting down the socket %i\n", count);
                    close(newSocket);
                }

                if(pthread_create(&tid2, NULL, receiveFunc,(void *)&newSocket) <0)
                {
                    printf("[-]Thread failed.\n");
                    printf("[-]Shutting down the socket %i\n", count);
                    close(newSocket);
                }
                
            }

        }

    }

    return 0;
}

void *handle_client(void *arg)
{
    while(1)
    {
        Pings Req, Res;
        Res.FrameType = 0x08;
        int client_socket = *((int*)arg);
    }
    return 0;


}





