#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <math.h>

clock_t start, stop;
struct tms startCPU, stopCPU;

char* RESULTSPATH="pomiar_zad_3.txt";

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

int readNumber(int file)//if given descriptor already is at finish function does nothing and return 0
{
    char* buf=buffer;
    if (!read(file,buf,1)) return -1;
    if (*buf=='\n') { printf("\n"); return -1; }

    while (1)
    {
        buf++;
        if (!read(file,buf,1) || *buf=='\n') break;
    }
    if (*buf=='\n') *(buf+1)='\0';
    else { *(buf+1)='\n'; *(buf+2)='\0'; }

    return atoi(buffer);
}

int countLines(int file)
{
    int lines=1;
    char buf;
    while (1)
    {
        if (!read(file,&buf,1)) break;
        if (buf=='\n') lines++;
    }
    if (buf=='\n') lines--;
    lseek(file,0,SEEK_SET);
    return lines;
}

int bCondition(int n)
{
    return n%10==7 || n%10==0 ? 1:0;
}

int cCondition(int n)
{
    return (int)sqrt((double)n)==sqrt((double)n)? 1:0;
}


int main(int argc, char* argv[]){
    char* file;
    if (argc == 2)  file=argv[1];
    else
    {
        file=malloc(50*sizeof(char));
        printf("Podaj litere, oraz plik dla ktorych wypisac linie z pliku, ktore zawieraja dana litere!\n");
        printf("Plik: ");
        scanf("%s",file);
    }
    saveHeader();
    startMeasure();
    int f=open(file,O_RDONLY);
    int f1=open("a.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR), f2=open("b.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR),
            f3=open("c.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    int numbers=countLines(f);
    int *tab = malloc(numbers*sizeof(int));

    for (int i=0;i<numbers;i++) tab[i]=readNumber(f);

    int even=0;
    char buff[64];

    for (int i=0;i<numbers;i++)
    {
        sprintf(buff,"%d\n",tab[i]);
        if (bCondition(tab[i])) write(f2,buff,strlen(buff));
        if (cCondition(tab[i])) write(f3,buff,strlen(buff));
        if (tab[i]%2==0) even++;
    }
    sprintf(buff,"%d",even);
    write(f1,buff,strlen(buff));
    free(tab);
    close(f);
    endMeasure("zadanie 3");

    return 0;
}
