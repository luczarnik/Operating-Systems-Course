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

void waitForProcesses(pid_t* processes, int size)
{
    for (int i=0;i<size;i++) waitpid(processes[i],NULL,0);
}

void clean()
{
    char ** arguments = malloc(2*sizeof(char*));
    arguments[0]=malloc(10*sizeof(char));
    strcpy(arguments[0],"cleaner");
    arguments[1]=NULL;
    printf("cos");
    execv("./cleaner",arguments);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr,"Invalid arguments number.\n");
        exit(-1);
    }
    atexit(clean);
    //generating keys
    int N=atoi(argv[1]), M=atoi(argv[2]);
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

    //creating shared memory and semaphoras
    owenID = shmget(owen_key,sizeof(struct Table),IPC_CREAT | 0666);
    tableID = shmget(table_key,sizeof(struct Table),IPC_CREAT | 0666);
    owen_sem = semget(owen_sem_key, 2,IPC_CREAT | 0666);
    table_sem = semget(table_sem_key,3,IPC_CREAT | 0666);

   

    setval(owen_sem,0,1);//semaphore to keep cooks not interapting with owen
    setval(owen_sem,1,MAXSIZE);//semaphore to ensure cook doesn't take action when oven is full
    setval(table_sem,0,0);//allowance to take pizza from table by deliver
    setval(table_sem,1,1);//allowance to put pizza on table for cook
    setval(table_sem,2,MAXSIZE);
    
    struct Table* owen = shmat(owenID,NULL,0);
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