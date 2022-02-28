
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>


int main()
{
    // creating results.txt file for consumer files :
    FILE* file = fopen("results.txt","a");
    fclose(file);
    mkfifo("pipe",S_IRUSR | S_IWUSR);

    char**  args= malloc(sizeof(char*)*5);
    for (int i=0;i<4;i++) args[i]= malloc(20*sizeof(char));
    strcpy(args[0],"consumer");
    strcpy(args[1],"./pipe");
    strcpy(args[2],"./results.txt");
    strcpy(args[3],"5");
    args[4]=NULL;

    pid_t consumersPid[3];
    for (int i=0 ;i<3;i++) 
    {
        consumersPid[i]=fork();
        if (consumersPid[i]==0) execvp("./consumer",args);
    }
    
     
    char**  args2= malloc(sizeof(char*)*6);
    for (int i=0;i<5;i++) args2[i]= malloc(20*sizeof(char));
    strcpy(args2[0],"./producent");
    strcpy(args2[1],"./pipe");
    strcpy(args2[2],"0");
    strcpy(args2[3],"./texts/file1.txt");
    strcpy(args2[4],"5");
    args2[5]=NULL;
    for (int i=0;i<7;i++)
    {
        if ( fork()== 0) execve("./producent",args2,NULL);
        args2[2][0]=(char)((int)(args2[2][0])+1);
        args2[3][12]=(char)((int)(args2[3][12])+1);
        
    }
    sleep(5);   
    for (int i=0;i<3;i++) kill(consumersPid[i],SIGKILL);
    for (int i=0;i<4;i++) free(args[i]);
    free(args);
    for (int i=0;i<5;i++) free(args2[i]);
    free(args2);
    return 0;
}