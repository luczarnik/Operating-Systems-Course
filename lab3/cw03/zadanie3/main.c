#include <stdio.h>
#include <expat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <math.h>
#include <dirent.h>


char path[1000];
char relativePath[1000];

int isTextFile(char* file);
int patternInFile(char* file, char* pattern);
void readFile(FILE* file,char** buffer);
int countCharacters(FILE* file);
void findFiles(char* path, char* pattern, int depth);
int occurs(char* buffer, char* pattern, int i);



int main(int argc, char* argv[]) {//the program is moving forward wave of processes. since we only print paths if there is nothing more to search we just abandon mother process
    if (argc !=4)
    {
        printf ("Nie poprawna liczba argumentow !");
        exit(1);
    }
    strcpy(path,argv[1]);
    //strcpy(path,".");
    strcpy(relativePath,"");
    int depth=atoi(argv[3]);//level 0 - current directory 1 - first directory and so on..
    //int depth=5;
    //char pattern[15]="file";
    findFiles(path,argv[2], depth);
    return 0;
}




int isTextFile(char* file)
{
    int length=(int)strlen(file);
    if (file[length-4]=='.' && file[length-3]=='t'&& file[length-2]=='x' && file[length-1]=='t')
        return 1;
    return 0;
}

int patternInFile(char* file, char* pattern)
{
    char filePath[1000];
    strcpy(filePath,path);
    strcat(filePath,"/"); strcat(filePath,file);
    FILE *f=fopen(filePath,"r");
    int length=countCharacters(f);
    char* buffer =malloc((length+1)*(int)(sizeof(char)));
    readFile(f,&buffer);

    for (int i=0;i<length;i++)
        if (occurs(buffer,pattern,i)) return 1;
    return 0;
}
void readFile(FILE* file,char** buffer)
{
    char* buf=*buffer;
    if (!fread(buf,sizeof(char),1,file)) return ;

    while (1)
    {
        buf++;
        if (!fread(buf,sizeof(char),1,file))break;
    }
    *(buf+1)='\0';
}

int countCharacters(FILE* file)
{
    int characters=0;
    char buf[2];
    while(fread(buf,sizeof(char),1,file))
        characters++;
    fseek(file,0,0);
    return characters;
}

int occurs(char* buffer, char* pattern, int i)//no need to check for outsdanging pattern outside buffer because at the end of string there is \0
//when it happens pattern is just not detected on place i
{
    int len=(int)strlen(pattern);
    for (int j=0;j<len;j++) {
        if (buffer[i + j] != pattern[j]) return 0;
    }
    return  1;
}

void findFiles(char* path, char* pattern,int depth) {//after checking every file and creating processes for subdirectories kills current process
    //printf("%s\n%s\n%d\n\n",path,pattern,depth);//testing
    if (depth<0) exit(0);
    DIR *dirp = opendir(path);
    if (dirp==NULL) { exit(0); }

    struct dirent *dir;
    char* dirname;
    while ( (dir = readdir(dirp)) !=NULL ) {
        dirname=dir->d_name;
        if (strcmp(dirname,"..")==0 || strcmp(dirname,".")==0) continue;

        if (isTextFile(dirname)) {//text files are only those ending with .txt anything other can be potentional library
            {
                if (patternInFile(dirname, pattern))
                    printf("The file was found in the location:\n%s/%s\nBy process with PID: %d\n\n", relativePath, dirname,
                           (int) getpid());
            }
        }
        else
        {
            pid_t child = fork();
            if (child==0) {
                strcat(path, "/"); strcat(relativePath,"/");
                strcat(path, dirname); strcat(relativePath, dirname);
                findFiles(path,pattern,depth-1);
            }
        }
    }
    closedir(dirp);

    exit(0);
}
