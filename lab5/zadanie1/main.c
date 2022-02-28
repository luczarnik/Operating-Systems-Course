#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "parser.h"

int main(int argc, char *argv[])
{
    char path[20]="arguments.txt";
     if (argc == 2)
    {
        printf("Invalid arguments number !");
        strcpy(path, argv[1]);
        exit(-1);
    }

    
    
    FILE *ptr;
    if (!(ptr = fopen(path, "r")))
    {
        printf("Nie udalo sie otworzyc pliku %s\n", path);
        exit(-1);
    }

    int *componentsNo = malloc(sizeof(int));
    *componentsNo = 0;
    struct Component *components = readComponents(ptr, componentsNo);


//1- wlot  0-wylot

    struct Stream* stream = readStream(ptr,components,*componentsNo);
    while (stream != NULL)
    {
        executeStream(stream,components);
        stream=readStream(ptr,components,*componentsNo);
    }

    for (int i=0;i<*componentsNo;i++) dealocateComponent(&components[i]);
    fclose(ptr);
    free(components);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //testing parser - reading components works properly
    /*
    for (int i=0;i<*componentsNo;i++)
    {
        printf("%s\n",components[i].name);
        for (int j=0;j<components[i].prog_no;j++)
        {
            printf(">>>>>>>>%s\n",components[i].programs[j].name);
            for (int k=0;k<=components[i].programs[j].arg_no;k++)
            {
                printf("-------------------%s\n", components[i].programs[j].arguments[k]);
            }
        }
    }

    struct Stream *stream = readStream(ptr,components,*componentsNo);  // reading stream works properly

    while (stream != NULL)
    {

        for (int i = 0; i < stream->componentsNo; i++)
        {

            printf("%s %d\n", components[stream->components[i]].name, stream->components[i]);
        }
        stream = readStream(ptr, components, *componentsNo);
    }

    /*

    int **fd=malloc(programsNumber*sizeof(int*));
    for (int i=0;i<programsNumber+1;i++)  
    {
        fd[i]=malloc(2*sizeof(int));
        pipe(fd[i]);
        close(fd[i][0]); close(fd[i][1]);
    }
    */
////////////////////////////////////////////////////////////////////////////

    return 0;
}
