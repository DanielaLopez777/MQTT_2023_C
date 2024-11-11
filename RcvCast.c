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

#include "libs.h"

pthread_mutex_t lockMutex;

int IoT[5] = {0};
int Sys[5] = {0};
int Est[5] = {0};

int cIoT=0, cSys=0, cEst=0; 

int client_socket;
Publish *pub;
Subscribe sus;

PubAck pubRes;
SubAck susRes;

void *receiveFunc(void *arg)
{
    int client_socket = *((int*)arg);
    PubRel rel;
    PubComp com;
    rel.FrameType = 0x60;

    while(1)
    {
        char buff[1024];
        
        if(recv(client_socket, &buff, sizeof(buff), 0) > 0)
        {
            
            pthread_mutex_lock(&lockMutex);
            
            if(buff[0] == 0x30) //Publish
            {
                //pthread_mutex_lock(&lockMutex);

                printf("[+]Publish received\n");
                pubRes.FrameType = 0x40;
                send(client_socket, &pubRes, sizeof(pubRes), 0);
                printf("[+]Publish Acknowledge sent\n");

               // pthread_mutex_unlock(&lockMutex);

                if(buff[2] == 3)
                {
                    
                    printf("[+]IoT selected\n");
                    for(int i=0; i<5; i++)
                    {
                        if(IoT[i] != 0)
                        {
                            //pthread_mutex_lock(&lockMutex);

                            send(IoT[i], &buff, sizeof(buff), 0);
                            printf("[+]Message sent to socket %i\n", IoT[i]);
                            send(client_socket, &rel, sizeof(rel), 0);
                            printf("[+]Publish release frame sent\n");
                            send(client_socket, &com, sizeof(com), 0);
                            printf("[+]Publish complete frame sent\n");

                            //pthread_mutex_unlock(&lockMutex);
                        }
                    }
                    
                }

                else if(buff[2] == 15)
                {
                    
                    printf("[+]Data Structures selected\n");
                    for(int i=0; i<5; i++)
                    {
                        if(Est[i] != 0)
                        {
                            //pthread_mutex_lock(&lockMutex);

                            send(Est[i], &buff, sizeof(buff), 0);
                            printf("[+]Message sent to socket %i\n", Est[i]);
                            send(client_socket, &rel, sizeof(rel), 0);
                            printf("[+]Publish release frame sent\n");
                            send(client_socket, &com, sizeof(com), 0);
                            printf("[+]Publish complete frame sent\n");

                            //pthread_mutex_unlock(&lockMutex);
                        }
                    }   

                }
                
                else if(buff[2] == 17)
                {
                    
                    printf("[+]Operative Systems selected\n");
                    for(int i=0; i<5; i++)
                    {
                        if(Sys[i] != 0)
                        {
                            //pthread_mutex_lock(&lockMutex);

                            send(Sys[i], &buff, sizeof(buff), 0);
                            printf("[+]Message sent to socket %i\n", Sys[i]);
                            send(client_socket, &rel, sizeof(rel), 0);
                            printf("[+]Publish release frame sent\n");
                            send(client_socket, &com, sizeof(com), 0);
                            printf("[+]Publish complete frame sent\n");

                            //pthread_mutex_unlock(&lockMutex);
                        }
                    }
                    
                }
            } 

            else if(buff[0] == 0x50) //Subscribe
            {
                printf("[+]Subscribe received\n");
                susRes.FrameType = 0x60;
                susRes.FrameLen = 0x03;
                susRes.PacketId = 0x0000;
                susRes.returnCode = 0x00;
                send(client_socket, &susRes, sizeof(susRes), 0);
                printf("[+]Subscribe Acknowledge sent\n");

                if(buff[2] == 3)
                {
                    printf("[+]IoT selected\n");
                    IoT[cIoT] = client_socket;
                    cIoT ++;
                }

                else if(buff[2] == 15)
                {
                    printf("[+]Data Structures selected\n");
                    Est[cEst] = client_socket;
                    cEst ++;  
                }
                
                else if(buff[2] == 17)
                {
                    printf("[+]Operative Systems selected\n");
                    Sys[cSys] = client_socket;
                    cSys ++;
                }

            }
            pthread_mutex_unlock(&lockMutex);
        }
    }

}