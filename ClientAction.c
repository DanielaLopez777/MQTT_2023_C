#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#include "libs.h"
pthread_mutex_t lockMutex;
extern int sock;

int client_socket, pubF=0, susF=0;

void *receiveClient(void *arg)
{
    char buff[1024];
    int client_socket = sock; //*((int*)arg);
    PubRec rec;
    rec.FrameType = 0x50;
    while(1)
    {
        if(recv(client_socket, &buff, sizeof(buff), 0) > 0)
        {
            if(buff[0] == 0x40) //PubAck
            {
                //pthread_mutex_lock(&lockMutex);

                printf("[+]Publish Acknowledge received\n");

                //pthread_mutex_unlock(&lockMutex);
            } 

            else if(buff[0] == 0x60) //SubAck
            {
                //pthread_mutex_lock(&lockMutex);

                printf("[+]Subscribe Acknowledge received\n");

                //pthread_mutex_unlock(&lockMutex);
            }

            else if(buff[0] == 0x30)
            {
                //pthread_mutex_lock(&lockMutex);

                printf("[+]New message received\n");
                send(client_socket, &rec, sizeof(rec), 0);
                printf("[+]Publish received frame sent\n");

                //pthread_mutex_unlock(&lockMutex);
            }
        }
    
    }

}


void *actionHandler(void *arg)
{
    int client_socket = sock;
    int action, top, count = 0;
    char message[100];
    char buffer[1024];
    Publish pub;
    PubAck pubRes;
    Subscribe sus;
    SubAck susRes;
    Disconnect dis;

    int suscriptions[3] = {0,0,0};

    char Temas[3][20] = {
        "IoT",
        "Data Structures",
        "Operative Systems"
    };

    while(1)
    {
        printf("\n\nPlease select the action you want to perform:\n1.Publish\n2.Suscribe\n3.Unsuscribe\n4.Disconnect\n");
        scanf("%i",&action);

        switch(action)
        {
            case 1:
                printf("Please select the topic where you want to publish:\n1.IoT\n2.Data Structures\n3.Operative Systems\n");
                scanf("%i", &top);

                printf("Type the message you want to publish\n");
                scanf("%s", message);

                pub.FrameType = 0x30;
                pub.FrameLen = 0x04 + strlen(Temas[top-1]) + strlen(message);
                pub.TopicLen = strlen(Temas[top-1]);
                strcpy(pub.Topic, Temas[top-1]);
                pub.MsgLen = strlen(message);
                strcpy(pub.Msg, message);

                if(send(client_socket, &pub, sizeof(pub), 0) <= 0) 
                {
                    printf("[-]The data was not sended\n");
                }

                else
                {
                    printf("[+]The message was successfully published\n");
                    pubF = 1;
                
                }
            break;

            case 2:
                printf("Please select the topic which you want to suscribe:\n1.IoT\n2.Data Structures\n3.Operative Systems\n");
                scanf("%i", &top); 

                suscriptions[top-1] = 1;

                sus.FrameType = 0x50;
                sus.FrameLen = 0x03 + strlen(Temas[top-1]);
                sus.TopicLen = strlen(Temas[top-1]);
                strcpy(sus.Topic, Temas[top-1]);

                if(send(client_socket, &sus, sizeof(sus), 0) <= 0) 
                {
                    printf("[-]The data was not sended\n");
                }

                else
                {
                    printf("[+]You were successfully suscribed\n");
                    count ++;
                    susF = 1;
                }
            break;

            case 4:
                dis.FrameType = 0xF0;
                send(client_socket, &dis, sizeof(dis), 0);
                close(client_socket);
            break;

            default:
                printf("Invalid option\n");
            break;
        }
    }
}
