#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void configureParent(char* task);
void signalHandler(int signalNr)
{
    printf("Received signal \n" );
}


int main(int argc, char* argv[]) {
    printf("%s",argv[2]);
    if (argc!=3 ){
        printf("Nie prawidlowa ilsoc argumentow");
        exit(1);
    }

    printf("\n\n----Proces rodzica -----\n\n");

    configureParent(argv[1]);
    raise(SIGUSR1);
    sigset_t mask;
    if ( strcmp(argv[1], "pending") == 0 || strcmp(argv[1], "mask") == 0 )
    {
        sigpending(&mask);
        printf("Signal pending: %d\n", sigismember(&mask, SIGUSR1));
    }

    if (strcmp(argv[2], "exec") == 0)   execl("./exec", "./exec", argv[1], NULL);
    else
    {

        pid_t child=fork();
        if (child==0)
        {
            printf("\n\n----Proces dziecka -----\n");
            if (strcmp(argv[1], "pending") != 0)    raise(SIGUSR1);

            if ( strcmp(argv[1], "pending") == 0 || strcmp(argv[1], "mask") == 0 )
            {
                sigpending(&mask);
                printf("Signal pending: %d\n", sigismember(&mask, SIGUSR1));
            }
            printf("\n\n\n");
        }
    }

    return 0;
}

void configureParent(char* task)
{
    if (strcmp(task,"ignore")==0)
    {
        signal(SIGUSR1,SIG_IGN);
    }

    else if (strcmp(task,"handler")==0)
    {
        signal(SIGUSR1,signalHandler);
    }

    else if (strcmp(task,"mask")==0 || strcmp(task,"pending")==0)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            printf("Signal blocking error");
            exit(1);
        }
    }

    else
    {
        printf("Invalid argument : %s!\n", task);
        exit(1);
    }
}
