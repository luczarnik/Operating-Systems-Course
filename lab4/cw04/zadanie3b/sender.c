#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int sentSignals=0;//sent signal
int acceptedSignals=0;
int countingSignal, terminationSignal;
char mode[30];
int toSend;
pid_t reciver;

void sendCountingSignal();
void sendTerminationSignal();

void signalHandler(int sigNo, siginfo_t *info, void *ucontext)
{
    if (sigNo==countingSignal)
    {
        acceptedSignals++;
        if (acceptedSignals < toSend) sendCountingSignal();
        else    sendTerminationSignal();
    }
    else
    {
        printf("sender otrzymal %d sygnalow, a powinien otrzmac %d\n",acceptedSignals,toSend);
        exit(0);
    }
}

void sendCountingSignal()
{
    sentSignals++;
     if (strcmp(mode,"kill")==0 || strcmp(mode,"sigrt")==0)
            kill(reciver,countingSignal);
        
    else
    {
        union sigval value;
        sigqueue(reciver, countingSignal, value);
    } 
}

void sendTerminationSignal()
{
     if (strcmp(mode,"kill")==0 || strcmp(mode,"sigrt")==0)
            kill(reciver,terminationSignal);
        
    else
    {
        union sigval value;
        sigqueue(reciver, terminationSignal, value);
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

    reciver =atoi(argv[1]);
    toSend = atoi(argv[2]);
    

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

    sendCountingSignal();


    while(1) { sigsuspend(&mask);}
    return 0;
}
