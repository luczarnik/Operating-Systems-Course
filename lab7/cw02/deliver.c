#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "table.h"
#include "utils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>


char buffer[30];

int main(int argc, char * argv[])
{
    key_t table_key = keygen('t');
    sem_t* table_sem0 = sem_open("tableSem0",O_RDWR | O_CREAT);
    sem_t* table_sem1 = sem_open("tableSem1",O_RDWR | O_CREAT);
    sem_t* table_sem2 = sem_open("tableSem2",O_RDWR | O_CREAT);
    int pizza;
    int tableID = shmget(table_key,sizeof(struct Table),0);

    while (1)
    {
        sem_wait(table_sem0);//waiting for pizzas on table
        sem_wait(table_sem1);//waiting for place by table
        struct Table* table = shmat(tableID,NULL,0);
        pizza=getPizza(table);
        printf("%d %s Wybieram pizze: %d Liczba pizz na stole: %d\n",getpid(), timestamp(buffer,30),pizza,table->elNo);
        sleep(4);
        printf("%d %s Dostarczm pizze %d.\n",getpid(),timestamp(buffer,30),pizza);
        shmdt(table);
        sem_post(table_sem2);
        sem_post(table_sem1);//stepping aside from table
        sleep(4);
    }
    return 0;
} 