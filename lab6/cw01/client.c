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

int serverQueueFD;
int clientQueueFD;
int friendQueueFD = -1;
int id;
int friendID=-1;

char buffer[BUFF_SIZE];
char command[20];
char msg[BUFF_SIZE];

void disconnect();

void getMessage(message_t *message)
{
    printf("************************************\n");
    printf("Client %d: %s\n", message->id, msg);
    printf("***********************************\n");
}

void acceptConnection(message_t *message)
{
    strcpy(buffer, message->text);
    interpret(buffer, command, msg);
    if (atoi(msg)==-1) return ;
    key_t friendKey = getKey(message->pid);
    if ((friendQueueFD = getQueue(friendKey)) == -1)
    {
        printf("Connection error: can't open request queue.\n");
        return;
    }
    friendID = message->id;
    printf("Connected with %d you are now in chat mode press [Enter] to check messages\n", friendID);
}

void finish()
{
    printf("Exiting.\n");
    if (friendID != -1) disconnect();
    message_t message;
    message.id=id;
    message.type=STOP;
    message.pid=getpid();
    send(serverQueueFD,&message);
    close(serverQueueFD);
    close(clientQueueFD);
    deleteQueue(clientQueueFD);
    exit(0);
}

void listener()
{
    while (!isEmpty(clientQueueFD))
    {
        message_t message;

        if (receiveNoWait(clientQueueFD, &message) != -1)
        {
            strcpy(buffer,message.text);
            interpret(buffer, command, msg);

            switch (message.type)
            {
            case STOP:
                finish();
                break;
            case CONNECT:
                acceptConnection(&message);
                break;
            case MESSAGE:
                getMessage(&message);
                break;
            case DISCONNECT:
                close(friendQueueFD);
                friendQueueFD = -1;
                friendID=-1;
                printf("Disconected.\n");
                break;
            default:
                break;
            }
        }
    }
}

void initialize()
{
    key_t serverKey = getServerKey();
    key_t clientKey = getClientKey();
    if ((serverQueueFD = getQueue(serverKey)) == -1)
    {
        perror("Cant accsess server queue!\n");
        exit(1);
    }

    if ((clientQueueFD = createQueue(clientKey)) == -1)
    {
        perror("Cant create clients queue!\n");
        exit(1);
    }

    message_t message;
    message.type = INIT;
    sprintf(message.text, "INIT %d", clientKey);
    message.pid = getpid();

    if (send(serverQueueFD, &message) == -1)
    {
        perror("unable to register");
        exit(1);
    }

    if (receive(clientQueueFD, &message) == -1)
    {
        perror("unable to register");
        exit(1);
    }

    id = message.id;
    printf("Client registered with id %d\n", id);
}

void listRequest()
{
    message_t message;
    message.type = LIST;
    message.id = id;
    message.pid = getpid();
    send(serverQueueFD, &message);
}

void sendMessage(char *msg)
{
    if (friendQueueFD == -1)
    {
        fprintf(stderr, "Can't send message, you are not connected with anybody.\n");
        return;
    }
    message_t message;
    message.type = MESSAGE;
    message.id = id;
    message.pid = getpid();
    sprintf(message.text, "MESSAGE %s", msg);
    send(friendQueueFD, &message);
}

void connect(int destinationID)
{
    printf("Connecting request:\nyour id: %d\ndestination id: %d [Enter] to check status\nType: MESSAGE [content] to send the message\n", id, destinationID);
    message_t message;
    message.type = CONNECT;
    message.id = id;
    message.pid = getpid();
    sprintf(message.text, "CONNECT %d", destinationID);
    send(serverQueueFD, &message);
}

void disconnect()
{
    if (friendID==-1)
    {
        printf("Trying to disconnect while not being conneted.\n");
        return;
    }
    message_t message;
    message.type = DISCONNECT;
    message.id = id;
    message.pid = getpid();
    sprintf(message.text, "DISCONNECT");
    send(friendQueueFD, &message);
    send(serverQueueFD,&message);
    close(friendQueueFD);
    friendQueueFD=-1;
    printf("Disconected from chat with %d, You don't receive messages anymore.\n",friendID);
    friendID=-1;
}

void sender()
{
    fgets(buffer, 1024, stdin);
    interpret(buffer, command, msg);

    if (strcmp("STOP", command) == 0)
       finish();
    else if (strcmp("LIST", command) == 0)
        listRequest();
    else if (strcmp("CONNECT", command) == 0)
        connect(atoi(msg));
    else if (strcmp("MESSAGE", command) == 0)
        sendMessage(msg);
    else if (strcmp("DISCONNECT", command) == 0)
        disconnect();
}

int main(int argc, char *argv[])
{

    signal(SIGINT,finish);

    initialize();

    while (1)
    {
        sender();
        listener();
    }

    return 0;
}