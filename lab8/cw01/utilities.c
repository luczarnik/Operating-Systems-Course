#include "utilities.h"
#include <stdio.h>

void reversePixel(int* value)
{
    *value=255-*value;
}

void valueSelecting(struct Argument* arg)
{
    for (int i=0;i<arg->height;i++)
    {
        for (int j=0;j<arg->width;j++)
        {
            if (arg->copyAccessor[i][j]<arg->upper && arg->copyAccessor[i][j]>=arg->lower)
            {
                reversePixel(arg->pgmAccessor[i]+j);
            }
        }
    }
}

void blockSelecting(struct Argument* arg)
{
    for (int i=0;i<arg->height;i++)
    {
        for (int j=arg->lower;j<arg->upper;j++)
        {
            reversePixel(arg->pgmAccessor[i]+j);
        }
    }
}