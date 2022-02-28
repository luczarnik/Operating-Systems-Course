#include "utils.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sem.h>

char* timestamp(char* buffer, int size)
{
    if (size<DATE_LENGTH +1) 
    {
        fprintf(stderr,"Timespec : To small buffer size! Required : %d\n"
        ,DATE_LENGTH+1);
        exit(-1);
    }

    struct timespec tm;
    timespec_get(&tm,TIME_UTC);
    strftime(buffer, size, "%D %T", gmtime(&tm.tv_sec));
    char* endptr= &(buffer[strlen(buffer)]);
    *endptr ='.';
    endptr++;
    long miliseconds = tm.tv_nsec/(long)(1000000);
    snprintf(endptr,size-DATE_LENGTH,"%ld UTC",miliseconds);
    return buffer;
}

key_t keygen(char ch)
{
    char* home = getenv("HOME");
    return ftok(home, ch);
}

void setval(int semID, int semNo, int value)
{
    if (value <0) 
    {
        fprintf(stderr,"Semaphore must be greater or equal than zero !!!\n");
        exit(-1);
    }

    union semun arg;
    arg.val =value;

    if (semctl(semID,semNo,SETVAL,arg) < 0 )
    {
        fprintf(stderr,"Error during assigning initial semaphore value.\n");
        exit(-1);
    }
}

void increment(int semID, int semNo)
{
    struct sembuf* buffer = malloc(sizeof(struct sembuf));
    buffer->sem_num=semNo;
    buffer->sem_op=1;
    buffer->sem_flg=0;
    if (semop(semID,buffer,1) < 0 )
    {
        fprintf(stderr,"Error during assigning initial semaphore value.\n");
        exit(-1);
    }
}

void decrement(int semID, int semNo)
{
    struct sembuf* buffer = malloc(sizeof(struct sembuf));
    buffer->sem_num=semNo;
    buffer->sem_op=-1;
    buffer->sem_flg=0;
    if (semop(semID,buffer,1) < 0 )
    {
        fprintf(stderr,"Error during assigning initial semaphore value.\n");
        exit(-1);
    }
}