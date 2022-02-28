#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int signalCount=0;
int countingSignal, terminationSignal;
char mode[30];
int toSend;

void signalHandler(int sigNo, siginfo_t *info, void *ucontext)
{
    if (sigNo==countingSignal)
    {
        signalCount++;
        if (!strcmp(mode,"queue")) 
        printf("Otrzymano sygnalow %d, a powinno dotrzec %d sygnalow.\n",
        signalCount,info->si_value.sival_int);
    }
    else
    {
        printf("sender otrzymal %d sygnalow, a powinien otrzmac %d\n",signalCount,toSend);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4 )
    {
        printf("Nie prawidlowa ilosc argumentow");
        exit(-1);
    }
    strcpy(mode,argv[3]);

    if (strcmp(argv[3],"kill")==0 || strcmp(argv[3],"queue")==0)
    {
        countingSignal=SIGUSR1;
        terminationSignal=SIGUSR2;
    }
    else if (strcmp(argv[3],"sigrt")==0)
    {
        countingSignal=SIGRTMIN+1;
        terminationSignal=SIGRTMIN+2;
    }
    else{
        printf("Nie prawidlowy tryb !\n");
        exit(-1);
    }

    pid_t reciver =atoi(argv[1]);
    toSend=atoi(argv[2]);
    if (strcmp(mode,"kill")==0 || strcmp(mode,"sigrt")==0)
    {
        for (int i = 0; i < toSend; ++i)
        {
            kill(reciver, countingSignal);
        }
        kill(reciver, terminationSignal);
    }
    else
    {
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < toSend; ++i)
        {
            sigqueue(reciver, countingSignal, value);
        }
        sigqueue(reciver, terminationSignal, value);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,countingSignal);
    sigdelset(&mask,terminationSignal);

    if (sigprocmask(SIG_BLOCK,&mask,NULL)<0)
    {
        printf("Nie udalo sie zablokowac sygnalow\n");
        exit(-1);
    }

    struct sigaction signalAct;
    signalAct.sa_sigaction = signalHandler;
    signalAct.sa_flags=SA_SIGINFO;

    sigfillset(&signalAct.sa_mask);
    sigdelset(&signalAct.sa_mask, countingSignal);
    sigdelset(&signalAct.sa_mask, terminationSignal);

    sigaction(countingSignal, &signalAct, NULL);
    sigaction(terminationSignal, &signalAct, NULL);


    while(1) { sigsuspend(&mask);}
    return 0;
}
