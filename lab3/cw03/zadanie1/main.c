#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char* argv[]) {
    if (argc!=2)
    {
    	printf("Nie prawidlowa ilosc argumentow!");
    	exit(1);
    }
    int n=atoi(argv[1]);
    pid_t parentPid=(int)getpid();
    int i=0;
    for (;i<n;i++)
    {
        if (parentPid!=(int)getpid())
            break;
        fork();
    }
    if ((int)getpid()!=parentPid)
        printf("Process ID procesu %d to : %d\n",i, (int)(getpid()));
    else printf("Parent pid to %d\n",parentPid);

    return 0;
}
