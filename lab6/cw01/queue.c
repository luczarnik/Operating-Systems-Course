#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include "message.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>


int send(int queue, message_t *message)
{
    if (msgsnd(queue, message, MAX_MESSAGE_SIZE, 0) == -1 )
    {
        fprintf(stderr,"Erorr during sending message with id: %d and content: %s.\n",
        message->id,message->text);
        return -1;
    }
    return 1;
}

int receive(int queue, message_t *message)
{
    return msgrcv(queue, message, MAX_MESSAGE_SIZE, -100, 0);
}
int receiveNoWait(int queue, message_t *message)
{
    return msgrcv(queue, message, MAX_MESSAGE_SIZE, -100, IPC_NOWAIT);
}

int createQueue(key_t key)
{
    return msgget(key, IPC_CREAT | IPC_EXCL | 0600);
}

int deleteQueue(int queue)
{
    return msgctl(queue, IPC_RMID, NULL);
}

int getQueue(key_t key)
{
    return msgget(key, 0);
}

int isEmpty(int queueId)
{
    struct msqid_ds buf;
    msgctl(queueId, IPC_STAT, &buf);

    return buf.msg_qnum == 0;
}

key_t getServerKey()
{
    key_t key;
    if ((key = ftok(getenv("HOME"), 1)) == -1)
    {
	    fprintf(stderr,"Can't generate server queue key!\n");
    }
    return key;
}

key_t getClientKey()
{
    key_t key;
    if ((key = ftok(getenv("HOME"), getpid())) == -1)
    {
        fprintf(stderr,"Can't generate clients queue key!\n");
    }
    return key;
}

key_t getKey(int arg)
{
    key_t key;
    if ((key = ftok(getenv("HOME"), arg)) == -1)
    {
	    fprintf(stderr,"Can't generate server queue key!\n");
    }
    return key;
}

void interpret(char * buffer,char* command, char* message)
{
    if (buffer[0] == '\0') return;
    
    char* ptr = buffer;
    while (*ptr != ' ' && *ptr !='\0' && *ptr != '\n') ptr++;
    if (*ptr == '\0' || *ptr == '\n') 
    {
        *ptr='\0';
        strcpy(command,buffer);
        return;
    }

    *ptr ='\0';
    ptr++;
    strcpy(command,buffer);
    strcpy(message,ptr);
}

