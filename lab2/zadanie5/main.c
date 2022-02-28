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

char* RESULTSPATH="pomiar_zad_5.txt";

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

void readFile(int file,char** buffer)//if given descriptor already is at finish function does nothing and return 0
{
    char* buf=*buffer;
    if (!read(file,buf,1)) return ;

    while (1)
    {
        buf++;
        if (!read(file,buf,1))break;
    }
    *(buf+1)='\0';
}

int countCharacters(int file)
{
    int characters=0;
    char buf;
    while(read(file,&buf,1))
        characters++;
    lseek(file,0,SEEK_SET);
    return characters;
}

void wrapp(char* str1, char* str2, int size)
{
    int i=0, j=0, characters=0;
    while (i<size)
    {
        if (characters==50)
        {
            characters=0;
            str2[j]='\n';
            j++;
        }
        str2[j]=str1[i];
        i++; j++; characters++;
    }
    str2[j]='\0';
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

    startMeasure();

    int f1, f2;
    f1=open(file1,O_RDONLY);
    f2=open(file2,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    int size=countCharacters(f1);
    char* str1=malloc((size+1)*sizeof(char));
    char* str2=malloc((size+10+size/50)*sizeof(char));
    readFile(f1,&str1);

    wrapp(str1,str2,size);
    write(f2,str2,strlen(str2));

    close(f1); close(f2);

    free(str1); free(str2);
    endMeasure("zadanie5");
    return 0;
}