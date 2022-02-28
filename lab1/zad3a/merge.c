//
// Created by luczarnik on 17.03.2021.
//

#include "merge.h"



struct Table createTable(int size)
{
    struct Table table;
    table.size=size;
    table.blocks=NULL;
    if (size ==0) return table;
    table.blocks = malloc(size*sizeof(struct Block));
    for (int i=0; i< size; i++) { table.blocks[i].lines=NULL; table.blocks[i].size=0;}
    return table;
}

struct Sequence createSequence(char* seq)//required form:
//names of files delimited by colons ended up with\0, pairs delimited by space
//sequence doesn't care about incorrect files names it only splits apart char sequence
//in given format
{
    char* copy=strdup(seq);

    int i=0;
    int size=1;
    while (copy[i]!='\0')//counting number of files
    {
        i++;
        if (copy[i]==' ') size++;
    }
    struct Sequence sequence;
    sequence.size=size;
    sequence.pairs = malloc(size* sizeof(struct Pair));

    char* ptr=strtok(copy," :");
    i=0;
    while(ptr != NULL)
    {
        sequence.pairs[i].fileA=ptr;
        ptr=strtok(NULL," :");
        sequence.pairs[i].fileB=ptr;
        ptr=strtok(NULL," :");
        i++;
    }
    return sequence;
}

int loadFile(char **buffer,const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        fprintf(stderr, "Can't read a file");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int fileLen = ftell(fp);//
    rewind(fp);

    *buffer = calloc(fileLen, sizeof(char));
    fread(*buffer, sizeof(char), fileLen, fp);
    fclose(fp);
    return fileLen;
}

int loadBlock(const char *file_name, struct Table table)
{
    for (int i = 0; i < table.size; i++)
    {
        if (table.blocks[i].lines == NULL)
        {
            char* buffer;
            int status = loadFile(&buffer, file_name);
            if (status < 0)
            {
                fprintf(stderr, "file loading error\n");
                return status;
            }
            table.blocks[i].size=linesNo(buffer);
            table.blocks[i].lines=malloc(table.blocks[i].size*sizeof(char*));

            char *ptr=strtok(buffer,"\n");
            for (int j=0;j<table.blocks[i].size;j++)
            {
                table.blocks[i].lines[j]=ptr;
                ptr=strtok(NULL,"\n");
            }
            return i;
        }
    }
    fprintf(stderr, "array is full\n");
    return -1;
}

int mixFiles(char *file1, char *file2, const char* tmp_file)
{
    int size3=0;//size of merged file
    char* buffer1, *buffer2;
    size3+=loadFile(&buffer1,file1);
    size3+=loadFile(&buffer2,file2);
    int size1=linesNo(buffer1), size2=linesNo(buffer2);
    char ** file1Lines= malloc(size1*sizeof(char*)), **file2Lines = malloc(size2*sizeof(char*));

    int i=0;    char* ptr=strtok(buffer1,"\n");
    int j=0;
    while (ptr!=NULL)
    {
        file1Lines[i]=ptr;
        ptr=strtok(NULL,"\n"); i++;
        j++;
    }

    i=0;        ptr=strtok(buffer2,"\n");
    while (ptr!=NULL)
    {
        file2Lines[i]=ptr;
        ptr=strtok(NULL,"\n"); i++;
    }

    i=1;
    char* buffer3=malloc((size3+2)*sizeof(char));//**
    strcpy(buffer3,file1Lines[0]);
    strcat(buffer3,"\n");
    strcat(buffer3,file2Lines[0]);
    strcat(buffer3,"\n");
    while (i<size1 && i<size2)
    {
        strcat(buffer3,file1Lines[i]);
        strcat(buffer3,"\n");
        strcat(buffer3,file2Lines[i]);
        strcat(buffer3,"\n");
        i++;
    }
    buffer3[size3]='\0';//** moze sie zdarzyc tak, ze strcat da na koncu za spacja null (jesli pliki maja tyle samo lini)
    // wtedy mamy na size3 miesjcu koniec lini, a na size3+1 null

    FILE* fp;
    fp=fopen(tmp_file,"w");
    fwrite(buffer3,sizeof(char),size3,fp);

    free(buffer1); free(buffer2); free(buffer3);
    fclose(fp);

    return 0;
}

void mergeSequence(struct Sequence sequence, struct Table table,int i,const char* tmp_file )
{
    for ( ;i<sequence.size;i++)
    {
        mixFiles(sequence.pairs[i].fileA,sequence.pairs[i].fileB,tmp_file);
    }
}

int linesNo(char* text2)
{
    char* text=strdup(text2);
    char* ptr=strtok(text,"\n");
    int lines=0;
    while (ptr != NULL)
    {
        lines++;
        ptr=strtok(NULL,"\n");
    }
    free(text);
    return lines;
}

int blockLinesNo(struct Table table ,int i)
{
    if (i<0 || i> table.size) return -1;
    return table.blocks[i].size;
}

void deleteBlock(struct Table table, int i)
{
    if (table.blocks[i].lines==NULL) return;// if the block is already empty do nothing

    free(table.blocks[i].lines[0]);//No need to free for every element o lines,[0] is sufficient
    //because these are pointers to parts of single allocated block of memory
    free(table.blocks[i].lines);//freeing table of pointers for lines
    table.blocks[i].size=0;
    table.blocks[i].lines=NULL;
}

void deleteLineInBlock(struct Table table, int line, int block)// doesn't free the memory, but table no longer see that lines
        // memory is freed when the block is deleted
{
    table.blocks[block].lines[line]=NULL;
    table.blocks[block].size--;
}

void printBlocks(struct Table table)
{
    for (int i=0;i<table.size;i++)
    {
        printf("Block number %d\n",(i+1));
        for (int j=0;j<table.blocks[i].size;j++)
        {
            printf("%s\n",table.blocks[i].lines[j]);
        }
        printf("\n\n");
    }
}

void clearTable(struct Table *table) //pointer required to change internal value
//in other function copying was sufficient because they were only changing some values which was
// pointed by pointers in structures not anything within block or table structures
{
    if (table->size == 0 ) return;
    for (int i=0;i<table->size;i++)
        deleteBlock(*table,i);
    free(table->blocks);
    table->size=0;
}
