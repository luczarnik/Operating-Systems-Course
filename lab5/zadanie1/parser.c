#define _GNU_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

char fileLine[1200];

int countCharsInLine(char *ptr, char character)
{
    int occurance = 0;
    while (*ptr != '\n' && *ptr != '\0')
    {
        if (*ptr == character)
            occurance++;
        ptr++;
    }
    return occurance;
}

void readLine(FILE *ptr) //reads line from file to global table
//important because otherwise would need constantly allocating and dealocating which is expensive
{
    char *ptr2 = fileLine;

    while (fread(ptr2, sizeof(char), 1, ptr))
    {
        if (*ptr2 == '\n')
            break;
        ptr2++;
    }
    if (*ptr2 != '\n')
    {
        *(ptr2 + 1) = '\n';
        *(ptr2 + 2) = '\0';
    }
    else
    {
        *(ptr2 + 1) = '\0';
    }
}

int isComponent(char *line) //checks formart to decide if we deal with component
{
    if (*line == '#' || *line == ' ' || *line == '\n')
        return 0;
    line++;
    while (*line != '\n' && *line != '\0')
    {
        if (*line == '=')
            return 1;
        if (*line == '|')
            return 0;
        line++;
    }
    return 0; //every other case
}

int isStream(char *line) //checks formart to decide if we deal with stream
{
    if (*line == '#' || *line == ' ' || *line == '\n')
        return 0;
    line++;
    while (*line != '\n' && *line != '\0')
    {
        if (*line == '=')
            return 0;
        if (*line == '|')
            return 1;
        line++;
    }
    return 1; //every other case
}

struct Program *readProgram(char *command)
{
    if (command == NULL)
        return NULL;

    struct Program *program = malloc(sizeof(struct Program));
    program->arg_no = countCharsInLine(command, ' ');
    program->arguments = malloc((program->arg_no + 2) * sizeof(char *));

    char *token = strtok(command, " "); //no need to worry anymore about substrings, so basic strtok is sufficient

    program->name = token;
    program->arguments[0] = malloc(sizeof((strlen(program->name) + 1) * sizeof(char)));
    strcpy(program->arguments[0], program->name);
    int i = 1;
    while (1) //last argument must be NULL in order to satisfy execvp later when program is used
    {
        token = strtok(NULL, " ");
        program->arguments[i] = token;
        i++;
        if (token == NULL)
            break;
    }
    return program;
}

struct Component *readComponent(FILE *ptr) //finds next line containing component loads it and moves ptr to next line
// returns loaded component
{
    int flag = 0;
    while (!feof(ptr))
    {
        readLine(ptr);
        if (isComponent(fileLine))
        {
            flag = 1;
            break;
        }
    }
    if (!flag)
        return NULL;

    char *line = malloc((strlen(fileLine) + 1) * sizeof(char)); //we dont worry about freeing this memory cuz it will be used for storing component info
    strcpy(line, fileLine);
    struct Component *component = malloc(sizeof(struct Component));
    component->prog_no = countCharsInLine(line, '|') + 1;
    component->programs = malloc(component->prog_no * sizeof(struct Program));

    char *ptr1 = NULL;
    char *token = strtok_r(line, "=", &ptr1); //reentrance version just in case
    token[strlen(token) - 1] = '\0';
    component->name = token;

    while (*token == ' ')
    {
        *token = '\0';
        token++;
    }

    char *bptr = &token[strlen(token) - 1]; //getting read of unneccesary spaces
    while (*bptr == ' ')
    {
        *bptr = '\0';
        bptr--;
    }

    line = strtok_r(NULL, "=", &ptr1);

    int i = 0;
    char *ptr2 = NULL;
    token = strtok_r(line, "|", &ptr2); //will be modifying substrings with strtoke, so strotok reantrance version is required
    while (token != NULL)
    {
        while (*token == ' ')
        {
            *token = '\0';
            token++;
        }
        bptr = &token[strlen(token) - 1];
        while (*bptr == ' ' || *bptr == '\n')
        {
            *bptr = '\0';
            bptr--;
        }
        component->programs[i] = *readProgram(token);
        token = strtok_r(NULL, "|", &ptr2);
        i++;
    }
    return component;
}

struct Component *readComponents(FILE *ptr, int *componentsNo)
{
    *componentsNo = 0;
    fseek(ptr, 0, 0);

    char *line = NULL;
    while (!feof(ptr))
    {
        readLine(ptr);
        if (isComponent(fileLine))
            (*componentsNo)++;
    }
    struct Component *components = malloc((*componentsNo) * sizeof(struct Component));

    fseek(ptr, 0, 0);
    for (int i = 0; i < *componentsNo; i++)
        components[i] = *readComponent(ptr);

    fseek(ptr, 0, 0);
    return (components);
}

struct Stream *readStream(FILE *ptr, struct Component *components, int componentsNo) //reads first stream after pointer
//and sets descriptor on next line
//if there is no stream in file returns NULL
{

    int flag = 0;
    while (!feof(ptr))
    {
        readLine(ptr);
        if (isStream(fileLine))
        {
            flag = 1;
            break;
        }
    }
    if (!flag && feof(ptr))
        return NULL;

    char *line = malloc(((strlen(fileLine) + 1) * sizeof(char)));
    strcpy(line, fileLine);
    struct Stream *stream = malloc(sizeof(struct Stream));
    stream->componentsNo = countCharsInLine(line, '|') + 1;
    stream->components = malloc(stream->componentsNo * sizeof(int));

    char *token = strtok(line, " |\n");

    for (int i = 0; i < stream->componentsNo; i++)
    {
        stream->components[i] = find(token, components, componentsNo);
        token = strtok(NULL, " |\n");
    }
    free(line);
    return stream;
}

int find(char *token, struct Component *components, int componentsNo)
{
    for (int i = 0; i < componentsNo; i++)
    {
        if (!strcmp(token, components[i].name))
            return i;
    }
    printf("Given bad value at the stream: %s was not defined !\n", token);
    exit(-1);
}

void dealocateComponent(struct Component *component)
{
    for (int i = 0; i < component->prog_no; i++)
        dealocateProgram(&component->programs[i]);
    free(component->name); //dealocates every program name, arguments contained, itself's name,
    //because those where allocatd together and later splittend with pointers
    free(component->programs);
}

void dealocateProgram(struct Program *program)
{
    free(program->arguments); // no need to worry about it's name, and arguments strings it will take component dealocation
    //shouldn't use individualy
}

void dealocateStream(struct Stream *stream)
{
    free(stream->components);
}

void executeStream(struct Stream *stream, struct Component *components)
{
    struct Component *component = connectStream(stream, components);

    executeComponent(component);

    dealocateComponent(component);
    free(component);
}

struct Component *connectStream(struct Stream *stream, struct Component *components)
{
    int progNo = 0;
    for (int i = 0; i < stream->componentsNo; i++)
        progNo += components[stream->components[i]].prog_no;

    struct Component *component = malloc(sizeof(struct Component));
    component->name = malloc(10 * sizeof(char));
    strcpy(component->name, "custom");

    component->prog_no = progNo;
    component->programs = malloc(progNo * sizeof(struct Program));

    int k = 0;

    struct Program *ptr;

    for (int i = 0; i < stream->componentsNo; i++)
    {
        for (int j = 0; j < components[stream->components[i]].prog_no; j++)
        {
            ptr = &(components[stream->components[i]].programs[j]);

            component->programs[k].name = malloc((strlen(ptr->name) + 1) * sizeof(char));
            strcpy(component->programs[k].name, ptr->name);

            component->programs[k].arg_no = ptr->arg_no;
            component->programs[k].arguments = malloc((ptr->arg_no + 2) * sizeof(char *));
            component->programs[k].arguments[ptr->arg_no + 1] = NULL;

            for (int l = 0; l <= ptr->arg_no; l++)
            {
                component->programs[k].arguments[l] = malloc((strlen(ptr->arguments[l]) + 1) * sizeof(char));
                strcpy(component->programs[k].arguments[l], ptr->arguments[l]);
            }
            k++;
        }
    }

    return component;
}

void executeComponent(struct Component *component)
{
    int prog_no = component->prog_no;
    int **fd = malloc(prog_no * sizeof(int *));
    for (int i = 0; i < prog_no; i++)
    {
        fd[i] = malloc(3 * sizeof(int));
        if (pipe(fd[i]) <0)
        {
            fprintf(stderr,"Pipe opening error !\n");
            exit(-1);
        }
    }

    for (int i = 0; i < prog_no; i++)
    {
        if (fork() == 0)
        {
            if (i>0)   
            {
                dup2(fd[i - 1][0], STDIN_FILENO);
                close(fd[i-1][1]);
            }

           if (i<prog_no-1) 
            {
                dup2(fd[i][1],STDOUT_FILENO);
                close (fd[i][0]);
            }
            else 
            {
                close(fd[i][0]);
                close(fd[i][1]);
            }

            for (int j = 0; j < prog_no && j!= i && j!= i-1; j++)
            {
                close(fd[i][0]);
                close(fd[i][1]);
            }
            execvp(component->programs[i].name,component->programs[i].arguments);
            exit(0);
        } 
    }

    for (int i=0;i<prog_no;i++)
    {
        close(fd[i][0]);
        close(fd[i][1]);
        free(fd[i]);
    }
    free(fd);

   for (int i=0;i<prog_no;i++) wait(0);
}
