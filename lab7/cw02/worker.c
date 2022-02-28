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


char timestmp[30];

int main()
{
    int pizza;
    srand(time(NULL));

    sem_t* owen_sem0 = sem_open("owenSem0",O_RDWR | O_CREAT);
    sem_t* owen_sem1 = sem_open("owenSem1",O_RDWR | O_CREAT);
    sem_t* table_sem0 = sem_open("tableSem0",O_RDWR | O_CREAT);
    sem_t* table_sem1 = sem_open("tableSem1",O_RDWR | O_CREAT);
    sem_t* table_sem2 = sem_open("tableSem2",O_RDWR | O_CREAT);
    
    
    while (1)
    {
        struct Table* owen = shmat(owenID,NULL,0);
        struct Table* table = shmat(tableID,NULL,0);
        pizza=rand()%9;
        printf("%d %s Przygotowuje pizze: %d.\n",getpid(),
        timestamp(timestmp,30),pizza);
        sleep(2);

        sem_wait(owen_sem1);//waiting till owen isn't full anymore
        sem_wait(owen_sem0);//ensuring it's only process by owen
        putPizza(owen,pizza);
        printf("%d %s Dodalem pizze: %d. Liczba pizz w piecu: %d.\n",getpid(),
        timestamp(timestmp,30),pizza,owen->elNo);
        sempost(owen_sem0);
        sleep(4);

        sem_wait(table_sem2);
        sem_wait(table_sem0);//waiting till can acess owen
        sem_post(owen_sem1);//increase pizzas slots in owen
        sem_wait(table_sem1);//waiting till can acces table
        pizza=getPizza(owen);
        putPizza(table,pizza);
        printf("%d %s Wyjmuje pizze: %d. Liczba pizz w piecu %d. Liczba pizz na stole: %d.\n",
        getpid(),timestamp(timestmp,30),pizza,owen->elNo,table->elNo);
        sem_post(owen_sem0);// stepping aside ofowen
        sem_post(table_sem0);//increasing pizzas on table
        sem_post(table_sem1);//stepping asid of table

        shmdt(table);
        shmdt(owen);
    }

    return 0;
}