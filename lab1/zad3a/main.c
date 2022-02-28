#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

#ifdef DLL
#include "dllmerge.h"
#include <dlfcn.h>
#endif

#ifndef DLL
#include "merge.h"
#endif

//Program handles the following comands:
// create_table size  - creates table given by size eventualy eareases previous table
// merge_files sequence -  merge files given in sequence and put them into table
// remove_block index  -removes block with given index
// remove_row block_index row_index - times --removes given row in given block
// every operation is time measured and it's result is stored in result.txt file

const char* RESULTSPATH="raport.txt";
static char* tmp_file="temporary.txt";



clock_t start, stop;
struct tms startCPU, stopCPU;


void startMeasure()
{
    start=times(&startCPU);
}

void endMeasure(char* name)
{
    stop=times(&stopCPU);
    int CPUCLOCKS=sysconf(_SC_CLK_TCK);
    double realTime = (double)(stop-start)/CPUCLOCKS;
    double userTime = (double)(stop-start)/CPUCLOCKS;
    double systemTime = (double)(stop-start)/CPUCLOCKS;

    FILE *f =fopen(RESULTSPATH,"a");
    fprintf(f,"%s\t\t\t%f\t%f\t%f\n", name, realTime,userTime,systemTime);
    fclose(f);
}

void checkArguments(int argc, int i, int count, char* command){
    // function checks only wheter number of arguments is correct. Correctness of arguments
    // is checked in library
    if (i+count>argc+1)
    {
        printf("Bad arguments Number for command: %s",command);
        exit(-1);
    }
}

void saveHeader()
{
    FILE *f =fopen(RESULTSPATH,"a");
    fprintf(f,"%s\t\t\t%s\t%s\t%s", "name ro    ", "realTime","userTime","systemTime\n");
    fclose(f);
}


int main(int argc, char* argv[]) 
{
    //first argument file location with sequence definition
    //second argument name of test // third argument is number of blocks to add and delete from table several times
    //forth argument is time measure option

    #ifdef DLL
    initializeFunctionsFromDynamicLibrary();
    #endif
    //if (argc!=5){printf("Nie prawiddlowa ilosc argumentow"); return -1;}

    struct Table table=createTable(0);    

    saveHeader();
    for (int i=1;i< argc;)
    {

        if (strcmp("create_table",argv[i])==0)
        {
            checkArguments(argc,i,1, "create_table");

            startMeasure();
          
            clearTable(&table);
            table = createTable(atoi(argv[i+1]));
            
            endMeasure("create_table");
            i+=2;
        }

        else if (strcmp("remove_row",argv[i])==0)
        {
            checkArguments(argc,i,3,"remove_row");
            startMeasure();

            for (int j=0;j<atoi(argv[i+3]);j++)   
            {             
                deleteLineInBlock(table,atoi(argv[i+2]),atoi(argv[i+1]));
            }

            endMeasure("remove_row");
            i+=4;
        }

        else if (strcmp("remove_block",argv[i])==0)
        {
            checkArguments(argc,i,1,"remove_block");
            startMeasure();

            deleteBlock(table,atoi(argv[i+1]));
            

            endMeasure("remove_block");
            i+=2;
        }
        else if (strcmp("merge_files",argv[i])==0)
        {
            checkArguments(argc,i,1,"merge_files");
            struct Sequence sequence;

            startMeasure();
            sequence=createSequence(argv[i+1]);
            mergeSequence(sequence,table,0,tmp_file);

            endMeasure("merge_files");

            startMeasure();
            loadBlock(tmp_file,table);
            endMeasure("load file to block");
            i+=2;	
        }
        
        else
        {
            clearTable(&table);
            printf("Nie prawidlowa komenda: %s",argv[i]);
            return -1;
        }
        

    }

    clearTable(&table);
    return 0;
}
