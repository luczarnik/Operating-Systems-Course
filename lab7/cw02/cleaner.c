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

int main(int argc, char* argv[])
{
    key_t owen_key = keygen('o');
    key_t owen_sem_key = keygen('1');
    key_t table_key = keygen('t');
    key_t table_sem_key = keygen('2');

    //deleting semaphoras if existed before
    int owenID = shmget(owen_key,sizeof(struct Table),0);
    int tableID = shmget(table_key,sizeof(struct Table),0);
    int owen_sem = semget(owen_sem_key, 2,0);
    int table_sem = semget(table_sem_key,3,0);
    semctl(owen_sem,0,IPC_RMID);
    semctl(table_sem,0,IPC_RMID);
    shmctl(owenID,IPC_RMID,NULL);
    shmctl(tableID,IPC_RMID,NULL);
    printf("Semaphoras and shared memory deleted.\n");
    
    return 0;
}