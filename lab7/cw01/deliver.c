#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>
#include "utils.h"
#include <unistd.h>
#include "table.h"

char buffer[30];

int main(int argc, char * argv[])
{
    key_t table_key = keygen('t');
    key_t table_sem_key = keygen('2');
    int pizza;
    int tableID = shmget(table_key,sizeof(struct Table),0);
    int table_sem = semget(table_sem_key,0,0);

    while (1)
    {
        decrement(table_sem,0);//waiting for pizzas on table
        decrement(table_sem,1);//waiting for place by table
        struct Table* table = shmat(tableID,NULL,0);
        pizza=getPizza(table);
        printf("%d %s Wybieram pizze: %d Liczba pizz na stole: %d\n",getpid(), timestamp(buffer,30),pizza,table->elNo);
        sleep(4);
        printf("%d %s Dostarczm pizze %d.\n",getpid(),timestamp(buffer,30),pizza);
        shmdt(table);
        increment(table_sem,2);
        increment(table_sem,1);//stepping aside from table
        sleep(4);
    }
    return 0;
} 