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


#define PORT 4444

#include "libs.h"

void timer_handler (int signum);
void *PingRequestFunc(void *arg);
void *SuscribeFunc(void *arg);
void *PublishFunc(void *arg);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
Connect enviado;
ConnAck recibido;
int sock = 0;



int main(int argc, char const *argv[])
{
    struct sockaddr_in serv_addr;
    
    enviado.FrameType = 0x10;
    enviado.MsgLen = 0x000F;
    enviado.UserLen = 0x0004;
    strcpy(enviado.User,"Dany");
    enviado.Version = 0x04;
    enviado.ConnectFlags = 0x20;
    enviado.KeepAlive = 0x0005;
    enviado.ClientIdLen = strlen("123");
    strcpy(enviado.ClientId,"123");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr("192.168.43.232");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed \n");
        return -1;
    }
        
    if(send(sock, &enviado, sizeof(enviado), 0) <= 0) 
    {
        printf("[-]The data was not sended\n");
        printf("[-]Shutting down the socket\n");
        //DISCONECT
        return -1;
    }

    memset(&recibido, 0, sizeof(recibido));

    int valread = recv(sock, &recibido, 1024, 0);
     printf("Frame type: 0x%02X\n", enviado.FrameType);
    if(valread <0)
    {
        printf("[-]The data was not received\n");
        printf("[-]Shutting down the socket...\n");
        return -1;
    }

    if(recibido.FrameType != 0x20 || recibido.MsgLen != 0x02 || recibido.Flags != 0x00 || recibido.ReturnStatus > 0x02)
    {
        printf("[-]Wrong data recieved\n");
        printf("[-]Shutting down the socket...\n");
        return -1;
    }

    else if(recibido.ReturnStatus == 0x01)
    {
        printf("[-]Client limit reached\n");
        printf("[-]Shutting down the socket...\n");
        return -1;
    }
    
    else if(recibido.ReturnStatus == 0x02)
    {
        printf("[-]The frame was malformed\n");
        printf("[-]Shutting down the socket...\n");
        return -1;
    }

    else
    {
        pthread_t PingReq, action, broad;

        if(pthread_create(&PingReq, NULL, PingRequestFunc,(void *)&sock) <0)
        {
            printf("[-]Ping request failed.\n");
            printf("[-]Shutting down the socket...\n");
            return -1;
        }

        printf("[+]SUCCESSFULLY CONECTED\n");


        if(pthread_create(&action, NULL, actionHandler,(void *)&sock) <0)
        {
            printf("[-]Action handler failed.\n");
            printf("[-]Shutting down the socket...\n");
            return -1;
        }

        if(pthread_create(&broad, NULL, receiveClient,(void *)&sock) <0)
        {
            printf("[-]Receive function failed.\n");
            printf("[-]Shutting down the socket...\n");
            return -1;
        }


        while(1);

    }
    return 0;
}

void *PingRequestFunc(void *arg)
{
    struct sigaction sa;
	struct itimerval timer;
	
	/* Install timer_handler as the signal handler for SIGVTALRM. */
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction (SIGVTALRM, &sa, NULL);
	
	/* Configure the timer to expire after 250 msec... */
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 250000;
	
	/* ... and every 250 msec after that. */
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 250000;
	
	/* Start a virtual timer. It counts down whenever this process is
	executing. */
	setitimer (ITIMER_VIRTUAL, &timer, NULL);
	
	/* Do busy work. */
	while (1)
	{

	}
}

void timer_handler (int signum)
{
    Pings PingRequest, PingResponse;
    PingRequest.FrameType = 0x09;
	static int count = 0;

	if(count%enviado.KeepAlive == 0)
    {
        send(sock, &PingRequest, sizeof(PingRequest), 0);
        /*if(recv(sock, &PingResponse, sizeof(PingResponse), 0) <= 0)
        {
            printf("[-]Ping request failed.\n");
            printf("[-]Shutting down the socket...\n");
            return;
        }*/
    }
			
}
