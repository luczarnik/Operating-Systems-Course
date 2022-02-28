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



char timestmp[30];

int main()
{
    key_t owen_key = keygen('o');
    key_t owen_sem_key = keygen('1');
    key_t table_key = keygen('t');
    key_t table_sem_key = keygen('2');
    int pizza;
    srand(time(NULL));

    int tableID = shmget(table_key,sizeof(struct Table),0);
    int table_sem = semget(table_sem_key,0,0);
    int owenID = shmget(owen_key,sizeof(struct Table),0);
    int owen_sem = semget(owen_sem_key,0,0);
    
    
    while (1)
    {
        struct Table* owen = shmat(owenID,NULL,0);
        struct Table* table = shmat(tableID,NULL,0);
        pizza=rand()%9;
        printf("%d %s Przygotowuje pizze: %d.\n",getpid(),
        timestamp(timestmp,30),pizza);
        sleep(2);

        decrement(owen_sem,1);//waiting till owen isn't full anymore
        decrement(owen_sem,0);//ensuring it's only process by owen
        putPizza(owen,pizza);
        printf("%d %s Dodalem pizze: %d. Liczba pizz w piecu: %d.\n",getpid(),
        timestamp(timestmp,30),pizza,owen->elNo);
        increment(owen_sem,0);
        sleep(4);

        decrement(table_sem,2);
        decrement(owen_sem,0);//waiting till can acess owen
        increment(owen_sem,1);//increase pizzas slots in owen
        decrement(table_sem,1);//waiting till can acces table
        pizza=getPizza(owen);
        putPizza(table,pizza);
        printf("%d %s Wyjmuje pizze: %d. Liczba pizz w piecu %d. Liczba pizz na stole: %d.\n",
        getpid(),timestamp(timestmp,30),pizza,owen->elNo,table->elNo);
        increment(owen_sem,0);// stepping aside ofowen
        increment(table_sem,0);//increasing pizzas on table
        increment(table_sem,1);//stepping asid of table

        shmdt(table);
        shmdt(owen);
    }

    return 0;
}