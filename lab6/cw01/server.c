#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "message.h"
#include "queue.h"

#define MAX_CLIENTS 64
#define MAX_GROUPS 16

int clients[MAX_CLIENTS][3]; //0 - clientFD    1- clientID    2-clientCallID
int clientsNo = 0;

char buffer[BUFF_SIZE];
char msg[BUFF_SIZE];
char command[20];

int serverQueueFD;

int newClientID()
{
    
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i][0] == -1) return i;

    return -1;
}

void clean() //closess IPCS
{
    if (clientsNo == 0) 
    {
        close(serverQueueFD);
        deleteQueue(serverQueueFD);
    }
    message_t message;
    strcpy(message.text,"STOP");
    message.type=STOP;
    for (int i=0;i<MAX_CLIENTS;i++)
        if (clients[i][0]!=-1)
        {
            printf("%d\n",i); 
            send(clients[i][0],&message);
        }

}

void initialize() // takes care of server queue
{
    key_t serverKey = getServerKey();
    if ((serverQueueFD = createQueue(serverKey)) == -1)
    {
        fprintf(stderr, "Queue creation error!\n");
    }
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        for (int j = 0; j < 3; j++)
            clients[i][j] = -1;
    }
}

void connect(message_t* messageIN)
{
    int clientID=messageIN->id;
    int destinationID=atoi(msg);

    message_t message;
    message.type = CONNECT;
    if (clientID == destinationID)
    {
        fprintf(stderr,"Client tried to connect with himself.\n");
        sprintf(message.text, "CONNECT %d", -1);
        send(clientID, &message);
        return;
    }

    if (destinationID >=MAX_CLIENTS || destinationID <0 )
    {
        fprintf(stderr,"Out of range connect destination");
        sprintf(message.text,"CONNECT %d", -1);
        send(clients[clientID][0],&message);
    }

    if (clients[destinationID][0] == -1 || clients[clientID][2] != -1 )
    {
        printf("%d\n", clients[clientID][2]);
        fprintf(stderr,"Connection between %d and %d feiled.\n",clientID,destinationID);
        return;
    }

    clients[destinationID][2]=clientID;
    clients[clientID][2]=destinationID;

    sprintf(message.text,"CONNECT %d",clientID);
    message.pid=clients[clientID][1];
    message.id=clientID;
    send(clients[destinationID][0],&message);

    sprintf(message.text,"CONNECT %d",destinationID);
    message.pid=clients[destinationID][1];
    message.id=destinationID;
    send(clients[clientID][0],&message);
    printf("connected clients %d and %d.\n",clientID,destinationID);
}


void registerClient(message_t* messageIN)
{
    int keyVal = messageIN->pid;
    int clientID = newClientID();

    if (clientID < 0)
    {
        fprintf(stderr, "Exceeded server clients capacity !!!\n");
        return;
    }

    if ((clients[clientID][0] = getQueue(getKey(keyVal))) == -1)
        fprintf(stderr, "Blad otwierania kolejki komunikatow dla nowego klienta! \n");
    
    clientsNo++;
    clients[clientID][1]=messageIN->pid;

    message_t message;
    message.type=INIT;
    message.id=clientID;
    message.pid=messageIN->pid;
    send(clients[clientID][0], &message);
    printf("Registered client %d. Current clients number: %d\n",clientID,clientsNo);
}

void listClients(message_t* message)
{
    int clientID = message->id;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i][1] >= 0)
        {
            if (i == clientID)
            {
                if (clients[i][2] == -1)
                {
                    printf("Client %d is available to connect (Me)\n", i);
                }
                else
                {
                    printf("Client %d is making a conversation right now (Me)\n", i);
                }
                continue;
            }
            if (clients[i][2]== -1)
            {
                printf("Client %d is available to connect\n", i);
            }
            else
            {
                printf("Client %d is making a conversation right now\n", i);
            }
        }
    }

}

void stop(message_t* message)
{
    clientsNo--;
    int clientId=message->id;
    printf("Client %d left. Remaining :%d\n",clientId,clientsNo);
    close(clients[clientId][0]);
    clients[clientId][0]=-1;
    clients[clientId][1]=-1;
    clients[clientId][2]=-1;
    if (clientsNo == 0)
    {
        printf("All clients left. Shouting down.\n");
        close(serverQueueFD);
        deleteQueue(serverQueueFD);
        exit(0);
    }

}


void disconnect(message_t* message)
{
    int id=message->id;
    int frinedId=clients[id][2];
    printf("Closing chat between %d and %d \n", id, frinedId);
    clients[id][2]=-1;
    clients[frinedId][2]=-1;
}



int main(int argc, char *argv[])
{
    signal(SIGINT,clean);

    initialize();

    message_t message;

    while (1)
    {
        if ((receive(serverQueueFD, &message)) == -1)
        {
            fprintf(stderr, "Receiving message error!\n");
            continue;
        }
        
        strcpy(buffer,message.text);
        interpret(buffer,command,msg);

        if (message.type == LIST)
            listClients(&message);
        else if (message.type == CONNECT)
            connect(&message);
        else if (message.type == DISCONNECT)
            disconnect(&message);
        else if (message.type == STOP)
            stop(&message);
        else if (message.type == INIT)
            registerClient(&message);
        else
            fprintf(stderr, "Klient %d podal nieprawidlowa komende! %ld\n",message.id,message.type);
    }

    return 0;
}