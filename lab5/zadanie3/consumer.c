#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void appendToFileAtLine(char* string, char* path, int line)//file under path must aready exist 
{
    FILE* file;
    if (!(file = fopen(path,"r+")))
    {
        printf("Failed to open destination file: %s\n!", path);
        exit(-1);
    };
    flock(file,LOCK_EX);
    fseek(file,0,SEEK_END);
    int fileSize=(int)ftell(file);
    fseek(file,0,SEEK_SET);

    int stringLength =strlen(string);
    int newStringLength = stringLength+fileSize+line+5;//initial string can contain only one line, then we have to put also return character signs

    char* fileContext = malloc(sizeof(char)*(fileSize+10));//+10 not +1 in order to avoid thinking about some odd cases, but 1 should be sufficient
    char* newFileContext = malloc(sizeof(char)  * (newStringLength +10));

    for (int i=0;i<fileSize+10;i++) fileContext[i]='\0';
    for (int i=0;i<newStringLength+10;i++) newFileContext[i]='\0';

    fread(fileContext,sizeof(char),fileSize,file);
    strcpy(newFileContext,fileContext);
    
    int linePtr=0; char* ptr=fileContext;
    while (linePtr<line)
    {
        linePtr++;
        ptr=strchr(ptr,'\n');
        if (ptr == NULL ) break;
        ptr++;
    }
    char* ptrCpy = ptr;// if we need to put string in middle of file we want to know
    //where to continue copying after we put  string inside file


    if (ptr == NULL)//if ptr is NULL, then at least one return character is missing
    //we copy file context to new file context and move ptr to end of old context,then
    //we add required end of line characters (at least one)
    {
        ptr=newFileContext+fileSize;
        *ptr='\n';
        ptr++;
        while (linePtr < line)
        {
            linePtr++;
            *ptr='\n';
            ptr++;
        }
        *ptr='\0';
    }
    while (*ptr!= '\0' && *ptr != '\n') ptr++;
    if (ptrCpy != NULL)  
    {
        ptr = newFileContext + (ptr - fileContext);//if ptrCpy!=NULL, them ptr points at fileContext string not the new one
        ptrCpy = strchr(ptrCpy,'\n');
        if (ptrCpy !=NULL) ptrCpy++;
    }
    strcpy(ptr, string);
    
    if (ptrCpy!=NULL)
    {
        while (*ptr!= '\0') ptr++;
        *ptr='\n';
        ptr++;
        strcpy(ptr,ptrCpy);
    }

    fseek(file,0,SEEK_SET);
    fwrite(newFileContext,sizeof(char),strlen(newFileContext),file);

    flock(file,LOCK_UN);
    fclose(file);
    free(fileContext);
    free(newFileContext);
    return;
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    { 
        printf ("Invalid arguments number ! \n");
        exit(-1);
    }
    FILE* pipe = fopen(argv[1],"r+");
    int N= atoi(argv[3]);
    int line;
    char *buffer = malloc((N+5)*sizeof(char));//first characeter of line is line number
    int length;
    while (1)
    {
        if (!(length=fread(buffer,sizeof(char),N+1,pipe)))
        {
            //when we read everything, try to read once again and no one is writing, then we
            //we need to open pipe once again 
            fclose(pipe);
            pipe=fopen("./pipe","r");
            continue;
        }
        buffer[length]='\0';
        line=(int)buffer[0]-(int)'0';
        appendToFileAtLine(buffer+1,argv[2],line);
    }
    
    fclose(pipe);
    free(buffer);
    return 0;
}