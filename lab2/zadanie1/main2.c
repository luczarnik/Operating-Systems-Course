#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/times.h>

clock_t start, stop;
struct tms startCPU, stopCPU;

char* RESULTSPATH="pomiar_zad_1.txt";

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

void saveHeader()
{
    FILE *f =fopen(RESULTSPATH,"a");
    fprintf(f,"%s\t\t\t%s\t%s\t%s", "name ro    ", "realTime","userTime","systemTime\n");
    fclose(f);
}

char buffer[1024];

int printLine(FILE* file)//if given descriptor already is at finish function does nothing and return 0
{
    char* buf=buffer;
    if (!fread(buf,sizeof(char),1,file)) return 0;
    if (*buf=='\n') { printf("\n"); return 1; }
    while (1)
    {
        buf++;
        if (!fread(buf,sizeof(char),1,file) || *buf=='\n') break;
    }
    if (*buf=='\n') *(buf+1)='\0';
    else { *(buf+1)='\n'; *(buf+2)='\0'; }
    printf("%s",buffer);
    return 1;
}

int main(int argc, char* argv[]){
    char* file1, *file2;
    if (argc == 3){   file1=argv[1];  file2=argv[2];}
    else
    {
        file1=malloc(50*sizeof(char));  file2=malloc(50*sizeof(char));
        printf("Podaj nazwy plikow !\n");
        printf("Plik 1:");
        scanf("%s",file1);
        printf("Plik 2:");
        scanf("%s",file2);
    }
    saveHeader();
    startMeasure();
    FILE *f1, *f2;
    f1=fopen(file1,"r");
    f2=fopen(file2,"r");

    while (printLine(f1)) if(!printLine(f2)) break;
    while (printLine(f1));
    while (printLine(f2));

    fclose(f1); fclose(f2);
    endMeasure("zadanie 1");

    return 0;
}
