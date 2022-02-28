#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char* argv[])
{
    if (argc != 5 )
    {
        printf ("Invalid arguments number ! \n");
        exit(-1);
    }
    FILE* pipe, *file;
    if (!(pipe = fopen("./pipe","w")))
    {
        printf("Couldn't open pipe!\n");
        exit(-1);
    }
    if (!(file = fopen(argv[3],"r")))
    {
        printf("Couldn't open source file: %s!\n",argv[3]);
        exit(-1);
    }
    int N= atoi(argv[4]);
    int line = atoi (argv[2]);
    char *buffer = malloc((N+2)*sizeof(char));//first characeter of line is line number
    int length;
    while ((length=fread(buffer+1,sizeof(char),N,file))==N)
    {
        sleep(1);
        buffer[0]=(char)(line+(int)'0');
        buffer[length+1]='\0';
        if (buffer[length]=='\n') buffer[length]='\0';
        fwrite(buffer,sizeof(char),length+1,pipe);
    }
    
    fclose(pipe);
    fclose(file);
    free(buffer);
    return 0;
}