#ifndef __UTILITIES__
#define __UTILITIES__
#include <sys/types.h>

char* timestamp(char*, int);
key_t keygen(char ch);
void setval(int semID, int semNo, int value);
void increment(int semID, int semNo);
void decrement(int semID, int semNo);
union semun 
{
    int val;   
    struct semid_ds *buf;  
    unsigned short *array; 
    struct seminfo  *__buf;
};

#define DATE_LENGTH 26

#endif