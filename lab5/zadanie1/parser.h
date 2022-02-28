#ifndef __PARSER__
#define __PARSER__
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct Program
{
    char* name;
    char** arguments;
    int arg_no;
};
struct Component
{
    char* name;
    struct Program* programs;
    int prog_no;
};

struct Stream
{
    int* components;
    int componentsNo;
};

void dealocateMemory();
struct Component* readComponents(FILE*,int*);
struct Stream* readStream(FILE* , struct Component*,int );
int isComponent(char*);
int isStream(char* );
struct Component* readComponent(FILE* line);// reads component from given line and moves file descriptor to another component
struct Program* readProgram(char*);
int find(char*, struct Component*, int);
void dealocateComponent(struct Component*);
void dealocateProgram(struct Program*);
void dealocateStream(struct Stream*);
void executeStream(struct Stream*, struct Component*);
struct Component *connectStream(struct Stream*, struct Component*);
void executeComponent(struct Component* component);


#endif