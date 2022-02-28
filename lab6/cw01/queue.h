#ifndef QUEUE_H
#define QUEUE_H
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "message.h"

int send(int queue, message_t *message);

int isEmpty(int queueId);

int receiveNoWait(int queue, message_t *message);

int createQueue(key_t key);

int deleteQueue(int queue);

int getQueue(key_t key);

int closeQueue(int queue);

int receive(int queue, message_t *message);

void interpret(char * buffer,char* command, char* message);

key_t getServerKey();
key_t getClientKey();
key_t getKey(int arg);



#define SOMETHING_HAPPEND SIGRTMIN

#endif
