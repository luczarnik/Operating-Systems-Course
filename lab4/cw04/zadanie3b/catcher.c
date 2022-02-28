#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


int signalCount=0;
int countingSignal, terminationSignal;
char mode[30];

void signalHandler(int sigNo, siginfo_t *info, void *ucontext)
{
    if (sigNo==countingSignal) 
    {
        signalCount++;
        if (strcmp(mode,"kill")==0 || strcmp(mode,"sigrt")==0) 
            kill(info->si_pid,countingSignal);
        else 
        {
            union sigval value;
            value.sival_int=0;
            sigqueue(info->si_pid,countingSignal,value);
        }
    }
    else
    {
        if (strcmp(mode,"kill")==0 || strcmp(mode,"sigrt")==0)
            kill(info->si_pid,terminationSignal);
        
        else
        {
            union sigval value;
            sigqueue(info->si_pid, terminationSignal, value);
        } 
        printf("Catcher otrzymal %d sygnalow\n",signalCount);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2 ) 
    {
        printf("Nie prawidlowa ilosc argumentow!\n");
        exit(-1);
    }   

    if (strcmp(argv[1],"kill")==0 || strcmp(argv[1],"queue")==0)
    {
        countingSignal=SIGUSR1;
        terminationSignal=SIGUSR2;
    }
    else if (strcmp(argv[1],"sigrt")==0)
    {
        countingSignal=SIGRTMIN+1;
        terminationSignal=SIGRTMIN+2;
    }
    else{
        printf("Nie prawidlowy tryb !\n");
        exit(-1);
    }
    strcpy(argv[1],mode);

    struct sigaction signalAct;
    signalAct.sa_sigaction = signalHandler;
    signalAct.sa_flags=SA_SIGINFO;

    sigfillset(&signalAct.sa_mask);
    sigdelset(&signalAct.sa_mask, countingSignal);
    sigdelset(&signalAct.sa_mask, terminationSignal);

    sigaction(countingSignal, &signalAct, NULL);
    sigaction(terminationSignal, &signalAct, NULL);

    printf("Pid catchera to %d\n",getpid());
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,countingSignal);
    sigdelset(&mask,terminationSignal);
    if (sigprocmask(SIG_BLOCK,&mask, NULL)<0) 
    {
        printf("Nie udalo sie zablokowac reszty sygnalow.]\n");
        exit(-1);
    }
    while(1) {sigsuspend(&mask);}
    return 0;
}