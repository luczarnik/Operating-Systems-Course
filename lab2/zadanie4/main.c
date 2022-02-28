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

char* RESULTSPATH="pomiar_zad_4.txt";

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

int occurs(char* buffer, char* pattern, int i)
{
    int len=strlen(pattern);
    for (int j=0;j<len;j++) {
        if (buffer[i + j] != pattern[j]) return 0;
    }
    return  1;
}

int append(char* wynik, char* pattern, int j)
{
    int len=strlen(pattern);
    for (int i=0;i<len;i++) wynik[j+i]=pattern[i];

    return len+j;
}

char* replace(char* buffer,char* pattern1, char* pattern2, int size)
{
    char *wynik =malloc((5*size+1)*sizeof(char));
    int i=0,j=0;

    while (i<size)
    {
        if (occurs(buffer,pattern1,i))
        {
            i+=strlen(pattern1);
            j=append(wynik,pattern2,j);
            continue;
        }

        wynik[j]=buffer[i];
        i++;    j++;
    }
    return wynik;
}

int main(int argc, char* argv[]){
    char* file1, *file2, *pattern1, *pattern2;
    if (argc == 5) {
        file1=argv[1];
        file2=argv[2];
        pattern1=argv[3];
        pattern2=argv[4];
    }
    else
    {
        file1=malloc(50*sizeof(char));
        file2=malloc(50*sizeof(char));
        pattern1=malloc(50*sizeof(char));
        pattern2=malloc(50*sizeof(char));
        printf("Podaj plik do wcytania, oraz do zapisu, do ktorego przepisze pierwszy plike zamieniajac dalej podany wzorzec kolejnym wzorcem\n");
        printf("Plik do wczytania: ");
        scanf("%s",file1);
        printf("Plik do zapisu: ");
        scanf("%s",file2);
        printf("zastepowany wzorzec: ");
        scanf("%s",pattern1);
        printf("wzorzec do wstawienia: ");
        scanf("%s",pattern2);
    }
    startMeasure();
    int f1=open(file1,O_RDONLY), f2=open(file2,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    int size=countCharacters(f1);
    char *buffer = malloc(size*sizeof(char)+1);

    readFile(f1, &buffer);
    
    char* wynik=replace(buffer,pattern1,pattern2,size);

    write(f2,wynik,strlen(wynik));

    close(f1);
    close(f2);

    free(buffer); free(wynik);
    endMeasure("zadanie 4");

    return 0;
}