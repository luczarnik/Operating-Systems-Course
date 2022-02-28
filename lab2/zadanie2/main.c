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

char* RESULTSPATH="pomiar_zad_2.txt";

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

int printLine(int file, char character)//if given descriptor already is at finish function does nothing and return 0
{
    char* buf=buffer;
    if (!read(file,buf,1)) return 0;
    if (*buf=='\n') { printf("\n"); return 1; }

    int flag=0;
    while (1)
    {
        if (*buf==character) flag=1;
        buf++;
        if (!read(file,buf,1) || *buf=='\n') break;
    }
    if (*buf=='\n') *(buf+1)='\0';
    else { *(buf+1)='\n'; *(buf+2)='\0'; }

    if (flag)   printf("%s",buffer);
    return 1;
}

int main(int argc, char* argv[]){
    char* file, character;
    if (argc == 3){   file=argv[2];  character=argv[1][0];}
    else
    {
        file=malloc(50*sizeof(char));
        printf("Podaj litere, oraz plik dla ktorych wypisac linie z pliku, ktore zawieraja dana litere!\n");
        printf("Litera: ");
        scanf("%c",&character);
        printf("Plik: ");
        scanf("%s",file);
    }
    saveHeader();
    startMeasure();

    int f1;
    f1=open(file,O_RDONLY);

    while (printLine(f1,character));

    close(f1);

    endMeasure("zadanie 2");

    return 0;
}
