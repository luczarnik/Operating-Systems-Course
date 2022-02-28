#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    printf("%s\n",argv[1]);
    printf("\n\n----Proces dziecka -----\n");
    if (strcmp(argv[1], "pending") != 0)    raise(SIGUSR1);

    if ( strcmp(argv[1], "pending") == 0 || strcmp(argv[1], "mask") == 0 )
    {
        sigset_t mask;
        sigpending(&mask);
        printf("Signal pending: %d\n", sigismember(&mask, SIGUSR1));
    }
    printf("\n\n\n");
    return 0;
}