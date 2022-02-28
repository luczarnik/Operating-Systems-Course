#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>


void handleSigint(int sig, siginfo_t *info, void *ucontext)
{
    printf("    ---- Sigint handler ---- \n");
    printf("Numer sygnalu: %d\n", info->si_signo);
    printf("PID procesu, ktory wyslal sygnal: %d\n", info->si_pid);
    printf("Rzeczywisty ID uzytkownika wysylajacego sygnal: %d\n", info->si_uid);
    if (info->si_code == SI_USER)
    {
        printf("Sygnal otrzymano z funkcji systemowej kill \n");
    }
    else if (info->si_code == SI_KERNEL)
    {
        printf("Sygnal doszedl od jadra\n");
    }
}

void handleChild(int sig, siginfo_t *info, void *ucontext)
{
    printf("    ---- Child handler ---- \n");
    printf("Numer sygnalu: %d\n", info->si_signo);
    printf("PID procesu, ktory wyslal sygnal: %d\n", info->si_pid);
    printf("Status z jakim zakonczyl sie proces potomka: %d\n", info->si_status);
}

void handleQueue(int sig, siginfo_t *info, void *ucontext)
{
    printf("    ---- Queue-Sigint handler ---- \n");
    printf("Numer sygnalu: %d\n", info->si_signo);
    printf("PID procesu, ktory wyslal sygnal: %d\n", info->si_pid);
    printf("Wartosc przeslana wraz z sygnalem: %d\n", info->si_value.sival_int);
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Nie prawidlowa liczba argumentow !!! \n");
        exit(-1);
    }
    int additionalFlag;
    if (!strcmp(argv[1],"SA_NOCLDWAIT"))
    {
         additionalFlag=SA_NOCLDSTOP;
         printf("\n\nTesting for flags: SIGINFO, NOCLDSTOP--------------------------------\n");
    }
    else if (!strcmp(argv[1],"SA_NOCLDSTOP")) 
    {
        additionalFlag=SA_NOCLDSTOP;
        printf("\n\nTesting for flags: SIGINFO, NOCLDWait------------------------------------\n");
    }
    else 
    {
        printf("Nie prawidlowa dodatkowa flaga\n");
        exit(-1);
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO | additionalFlag;

    printf(" ------ Testing for SIGCHLD ----------\n");
    action.sa_sigaction = handleChild;
    sigaction(SIGCHLD, &action, NULL);
    pid_t child_pid = fork();

    if (child_pid == 0)   exit(67);

    wait(NULL);

    
    printf("\n\n ----- Testing for SIGINT sent by sigqueue -----\n");
    action.sa_sigaction = handleQueue;
    sigaction(SIGINT, &action, NULL);

    union sigval sigval;
    sigval.sival_int = 33;
    sigqueue(getpid(), SIGINT, sigval);


    printf("\n\n ----- Testing for SIGINT sent by kill -----\n");
    action.sa_sigaction = handleSigint;
    sigaction(SIGINT, &action, NULL);
    kill(getpid(),SIGINT);

    return 0;
}