#include "pbma/pbma_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utilities.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
    
    if (argc != 5)
    {
        fprintf(stderr,"Nie prawidlowa liczba argumentow !");
        exit(-1);
    }
    //---------------Reading Image section --------------------------------------
    char inputPath[20] = "images/input.pgm";
    strcpy(inputPath,argv[3]);
    char resultPath[20] = "images/output.pgm";
    strcpy(resultPath,argv[4]);
    int *pgmImage, width, height, spectrum;
    pbma_read(inputPath, &width, &height,&spectrum, &pgmImage);
    int **imageAccessor = malloc(height * sizeof(int *)); // 2d access to image pixels
    int **copyAccessor=malloc(height * sizeof(int *));//read onyle version required, because original changes


    int *copyImage=malloc(width*height*sizeof(int));
    for (int i=0; i<width*height;i++) copyImage[i]=pgmImage[i];

    for (int i = 0; i < height; i++)
        imageAccessor[i] = pgmImage + i * width;
    for (int i = 0; i < height; i++)
        copyAccessor[i] = copyImage + i * width;

    /*//////////////image accesor testing
    imageAccesor[1][1]=-5;
    imageAccesor[1][3]=-5;
    */
    /////////////////////////////////////////////////////////////////////////////////

    int threadsNo = atoi(argv[1]);
    enum MODE mode;
    if (strcmp(argv[3],"block")) mode=block;
    else if (strcmp(argv[3]),"numbers") mode = value;

    int valueChank = spectrum / threadsNo;
    int blockWidth = width / threadsNo;
    struct Argument *args = malloc(threadsNo * sizeof(struct Argument));
    pthread_t* threadIDS = malloc(threadsNo * sizeof(pthread_t));

    for (int i = 0; i < threadsNo - 1; i++) //last iteration we take separetly because threads number can be not divisible
    {
        args[i].pgmAccessor = imageAccessor;
        args[i].copyAccessor=copyAccessor;
        args[i].width = width;
        args[i].height = height;
        if (mode == value)
        {
            args[i].lower = i * valueChank;
            args[i].upper = (i + 1) * valueChank;
            pthread_create(threadIDS+i,NULL,valueSelecting,(void*)(args+i));
        }
        else
        {
            args[i].lower = i * blockWidth;
            args[i].upper = (i + 1) * blockWidth;
            pthread_create(threadIDS+i,NULL,blockSelecting,(void*)(args+i));
        }

    }
    args[threadsNo-1].pgmAccessor = imageAccessor;
    args[threadsNo-1].copyAccessor = copyAccessor;
    args[threadsNo-1].width = width;
    args[threadsNo-1].height = height;
    if (mode == value)
    {
        args[threadsNo-1].lower = (threadsNo-1) * valueChank;
        args[threadsNo-1].upper=256;
        pthread_create(threadIDS+threadsNo-1,NULL,valueSelecting,(void*)(args+threadsNo-1));
    }
    else
    {
        args[threadsNo-1].lower = (threadsNo-1) * blockWidth;
        args[threadsNo-1].upper = width-1;
        pthread_create(threadIDS+threadsNo-1,NULL,blockSelecting,(void*)(args+threadsNo-1));
    }

    for (int i=0;i<threadsNo;i++) pthread_join(threadIDS[i],NULL);


    pbma_write(resultPath, width, height,spectrum, pgmImage);
    free(imageAccessor);
    free(copyAccessor);
    free(copyImage);
    free(pgmImage);
    free(threadIDS);

    return 0;
}
