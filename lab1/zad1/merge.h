//
// Created by luczarnik on 17.03.2021.
//

#ifndef UNTITLED_MERGE_H
#define UNTITLED_MERGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

struct Table createTable(int size);
struct Sequence createSequence(char* seq);
int loadFile(char **buffer, char *file_path);
int file_to_array(char *file_name);
int mixFiles(char *file1, char *file2, char* tmp_file);
int linesNo(char* text);
void deleteBlock(struct Table table, int i);
int blockLinesNo(struct Table table ,int i);
void deleteLineInBlock(struct Table table, int line, int block);
void printBlocks(struct Table table);
void mergeSequence(struct Sequence sequence, struct Table table ,int i);

#endif //UNTITLED_MERGE_H

