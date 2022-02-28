#ifndef __MESSAGE__
#define __MESSAGE__

#include <time.h>
#define BUFF_SIZE 1024

typedef struct message_t
{
    long type;
    time_t timestamp;
    int id;
    char text[BUFF_SIZE];
    int pid;
} message_t;

#define MAX_MESSAGE_SIZE sizeof(message_t) - sizeof(long)
#define ALL_MESSAGE_SIZE sizeof(message_t)
#define MAX_MESSAGES 10

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5
#define MESSAGE 6


#endif


