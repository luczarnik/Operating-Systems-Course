//
// Created by luczarnik on 18.03.2021.
//
#ifndef UNTITLED_DLLMERGE_H
#define UNTITLED_DLLMERGE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>


struct Pair
{
    char* fileA;
    char* fileB;
};

struct Block
{
    int size;
    char** lines;//null if empty block
};

struct Sequence
{
    int size;
    struct Pair* pairs;
};

struct Table{
    int size;
    struct Block* blocks;
};

struct Table (*createTable)(int size);
int (*loadBlock)(const char *file_name, struct Table table);
struct Sequence (*createSequence)(char* seq);
int (*loadFile)(char **buffer, const char *file_path);
int (*file_to_array)(char *file_name);
int (*mixFiles)(char *file1, char *file2, char* tmp_file);
int (*linesNo)(char* text);
void (*deleteBlock)(struct Table table, int i);
int (*blockLinesNo)(struct Table table ,int i);
void (*deleteLineInBlock)(struct Table table, int line, int block);
void (*printBlocks)(struct Table table);
void (*mergeSequence)(struct Sequence sequence, struct Table table ,int i,const char* tmp_file);
static void* handle=NULL;

void initializeFunctionsFromDynamicLibrary()
{
    handle=dlopen("./libmerge.so",RTLD_NOW);
    if (handle==NULL)
    {
        printf("Nie zaladowano bibliotek");
        return;
    }
    
    loadBlock = dlsym(handle,"loadBlock");
    createTable = dlsym(handle, "createTable");
    createSequence = dlsym(handle, "createSequence");
    loadFile = dlsym(handle, "loadFile");
    file_to_array = dlsym(handle, "file_to_array");
    mixFiles = dlsym(handle, "mixFiles");
    linesNo = dlsym(handle, "linesNo");
    deleteBlock = dlsym(handle, "deleteBlock");
    blockLinesNo = dlsym(handle, "blockLinesNo");
    deleteLineInBlock = dlsym(handle, "deleteLineInBlock");
    printBlocks = dlsym(handle, "printBlocks");
    mergeSequence = dlsym(handle, "mergeSequence");
    char *error;
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}	


#endif //UNTITLED_DLLMERGE_H
