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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

void waitForProcesses(pid_t* processes, int size)
{
    for (int i=0;i<size;i++) waitpid(processes[i],NULL,0);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr,"Invalid arguments number.\n");
        exit(-1);
    }

    int owenID = shm_open("owen",O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    int tableID = shm_open("table",O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    ftruncate(owenID,sizeof(struct Table));
    ftruncate(tableID,sizeof(struct Table));

    sem_t* owen_sem0 = sem_open("owenSem0",O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    sem_t* owen_sem1 = sem_open("owenSem1",O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, MAXSIZE);
    sem_t* table_sem0 = sem_open("tableSem0",O_RDWR | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO,0);
    sem_t* table_sem1 = sem_open("tableSem1",O_RDWR | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO,1);
    sem_t* table_sem2 = sem_open("tableSem2",O_RDWR | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO,MAXSIZE);
    
    struct Table* owen = mmap(NULL,sizeof(struct Table)))]
    struct Table* table = shmat(tableID,NULL,0);
    initialize(owen);
    initialize(table);
    shmdt(table);
    shmdt(owen);

    char** worker=malloc(2*sizeof(char*));
    worker[0]=malloc(20*sizeof(char));
    strcpy(worker[0],"worker");
    worker[1]=NULL;
    char** deliver=malloc(2*sizeof(char*));
    deliver[0]=malloc(20*sizeof(char));
    strcpy(worker[0],"deliver");
    deliver[1]=NULL;
    

    pid_t* workersPID = malloc(N*sizeof(int));
    pid_t* deliversPID = malloc(M*sizeof(int));
    // asigning proper arguments
    for (int i=0;i<N;i++)
    {
        sleep(1);
        workersPID[i]=fork();
        if (workersPID[i] == 0 )
            if (!execvp("./worker",worker))
            {
                fprintf(stderr,"Couldn't execute worker process.\n");
                exit(-1);
            }
    }
    for (int i=0;i<M;i++)
    {
        deliversPID[i]=fork();
        if (deliversPID[i] == 0)
            if (!execvp("./deliver",deliver))
            {
                fprintf(stderr,"Couldn't execute deliver process.\n");
                exit(-1);
            }
            
    }

    waitForProcesses(deliversPID,M);
    waitForProcesses(workersPID,N);
    free(workersPID);
    free(deliversPID);
    free(worker[0]);
    free(worker);
    free(deliver[0]);
    free(deliver);
    return 0;
}